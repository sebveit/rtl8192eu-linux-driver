#!/bin/bash
# Script to replace sprintf with snprintf in the RTL8192EU driver
# This improves security by preventing buffer overflows

set -e

echo "Starting sprintf modernization..."

# Function to process a file
process_file() {
    local file="$1"
    local tmp_file="${file}.tmp"
    local changes=0
    
    # Check if file exists
    if [ ! -f "$file" ]; then
        return 0
    fi
    
    # Create a backup
    cp "$file" "${file}.bak"
    
    # Process the file with perl for more complex replacements
    perl -pe '
        # Handle sprintf(buf, ...) -> snprintf(buf, sizeof(buf), ...)
        # This pattern matches most common cases
        if (/(\s*)sprintf\s*\(\s*([^,]+),/) {
            my $indent = $1;
            my $buffer = $2;
            
            # Skip if buffer looks like a pointer arithmetic expression
            if ($buffer !~ /\+|\-|\*|\/|\[.*\]/) {
                # Simple buffer name
                s/sprintf\s*\(\s*([^,]+),/snprintf($1, sizeof($1),/;
            } elsif ($buffer =~ /^(\w+)\s*\+\s*(\w+)$/) {
                # Buffer with offset like buf + offset
                my $base = $1;
                s/sprintf\s*\(\s*([^,]+),/snprintf($1, sizeof($base) - ($2),/;
            } elsif ($buffer =~ /pstr\((\w+)\)/) {
                # Special case for pstr(s) pattern
                s/sprintf\s*\(pstr\((\w+)\),/snprintf(pstr($1), pstr_len($1),/;
            }
        }
    ' "$file" > "$tmp_file"
    
    # Check if changes were made
    if ! diff -q "$file" "$tmp_file" > /dev/null 2>&1; then
        mv "$tmp_file" "$file"
        echo "Updated: $file"
        ((changes++))
    else
        rm "$tmp_file"
        rm "${file}.bak"
    fi
    
    return $changes
}

# Process all C files in core/, hal/, and os_dep/
total_files=0
updated_files=0

for dir in core hal os_dep; do
    if [ -d "$dir" ]; then
        echo "Processing $dir/..."
        while IFS= read -r -d '' file; do
            ((total_files++))
            if process_file "$file"; then
                ((updated_files++))
            fi
        done < <(find "$dir" -name "*.c" -print0)
    fi
done

echo "Modernization complete!"
echo "Total files processed: $total_files"
echo "Files updated: $updated_files"

# Now handle special cases that need manual review
echo ""
echo "Finding remaining sprintf calls that need manual review..."
grep -r "sprintf" --include="*.c" core/ hal/ os_dep/ 2>/dev/null | head -20 || true

echo ""
echo "Note: Some sprintf calls may need manual review, especially those with:"
echo "  - Complex buffer calculations"
echo "  - Pointer arithmetic"
echo "  - Dynamic buffer sizes"
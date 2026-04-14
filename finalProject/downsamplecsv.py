

def copy_every_tenth_line(input_file_path, output_file_path):
    """
    Reads the input file and writes every 10th line to the output file.
    """
    try:
        with open(input_file_path, 'r', encoding='utf-8') as infile, \
             open(output_file_path, 'w', encoding='utf-8') as outfile:
            
            # Start counting from 1 to easily identify multiples of 10
            for line_number, line in enumerate(infile, start=1):
                if line_number % 10 == 0:
                    outfile.write(line)
                    
        print(f"Successfully copied every 10th line to {output_file_path}")
        
    except FileNotFoundError:
        print("Error: The source file was not found.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

# Example usage:
copy_every_tenth_line('simulation_7_ds.csv', 'simulation_7_ds_ds.csv')
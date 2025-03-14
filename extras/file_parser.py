import os
import json

def format_title(filename_prefix):
    """
    Formats the title by replacing underscores with spaces and capitalizing each word.
    """
    return " ".join(word.capitalize() for word in filename_prefix.split("_"))

def process_lox_files(directory):
    """
    Reads all .lox files in the given directory and creates a structured list of dictionaries.
    """
    lox_entries = []
    
    for filename in os.listdir(directory):
        if filename.endswith(".lox"):
            file_path = os.path.join(directory, filename)
            with open(file_path, "r", encoding="utf-8") as file:
                content = file.read()
            
            if filename.startswith("test_"):
                title = format_title(filename[5:-4])  # Remove 'test_' and '.lox'
                description = None  # No description for test cases
            elif filename.startswith("bench_"):
                title = format_title(filename[6:-4]) + " Benchmark"
                description = "One of the benchmarks used during development. Warning: slow! Might take a few seconds to run."
            else:
                continue  # Skip files that don't match the expected pattern
            
            entry = {"title": title, "code": content}
            if description:
                entry["description"] = description
            
            lox_entries.append(entry)
    
    return lox_entries

if __name__ == "__main__":
    directory = input("Enter the path to the folder containing .lox files: ")
    parsed_data = process_lox_files(directory)
    print(json.dumps(parsed_data, indent=2))

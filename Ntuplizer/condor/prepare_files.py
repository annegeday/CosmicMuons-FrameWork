import os
import subprocess

# Define inputs and outputs
datasets = [
    '/Cosmics/Commissioning2025-PromptReco-v1/AOD', 
   # '/Cosmics/Commissioning2025-PromptReco-v2/AOD'
]
output_base = 'eos/user/a/aovergaa/SummerStudent_Project/CMSSW_15_0_5/src/CosmicMuons-FrameWork/Ntuplizer/test/condor'
output_txt_file = 'files.txt'

# Open the text file to write the results
with open(output_txt_file, 'w') as txt_file:
    for dataset in datasets:
        # 1. Extract the folder name
        # dataset.split('/') for '/Cosmics/Commissioning.../AOD' gives:
        # ['', 'Cosmics', 'Commissioning...', 'AOD']
        # The second element between slashes is at index 2
        parts = dataset.split('/')
        if len(parts) < 3:
            print(f"Skipping invalid dataset format: {dataset}")
            continue
            
        folder_name = parts[2]
        
        # 2. Create the output folder
        out_folder = os.path.join(output_base, folder_name)
        os.makedirs(out_folder, exist_ok=True)
        print(f"Ensured directory exists: {out_folder}")
        
        # 3. Use dasgoclient to get the list of files
        query = f"file dataset={dataset}"
        print(f"Querying dasgoclient for {dataset}...")
        
        try:
            # Run dasgoclient and capture the output
            result = subprocess.run(['dasgoclient', '-query', query], 
                                    capture_output=True, text=True, check=True)
            
            # Split the output by line to get a list of file paths
            files = result.stdout.splitlines()
            
            # 4. Write pairs to the .txt file
            for f in files:
                f = f.strip()
                if not f:
                    continue # Skip empty lines
                
                # Extract the last element of the file path
                file_name = f.split('/')[-1]
                
                # Construct the full target path
                target_path = os.path.join(out_folder, file_name)
                
                # Write the pair separated by a space
                txt_file.write(f"{f} {target_path}\n")
                
            print(f"  -> Successfully found and mapped {len(files)} files.\n")
            
        except subprocess.CalledProcessError as e:
            print(f"Error running dasgoclient for {dataset}:\n{e.stderr}")
        except FileNotFoundError:
            print("Error: 'dasgoclient' command not found. Please run 'cmsenv' first.")
            break # Exit the loop as subsequent calls will also fail

print(f"Done! The file pairs have been saved to '{output_txt_file}'.")

# Your choice of number of muons + events

nMuons_list = [1, 2, 3]
output_txt_file = 'sim_input.txt'

nEvents_constant = True #set this to false if you want different number of events per simulation run

nEvents = 50
nEvents_list = [1000, 1000, 1000, 200, 200, 50] # indices should match nMuons_list


if nEvents_constant:
   with open(output_txt_file, 'w') as txt_file:
      for n in nMuons_list:
         txt_file.write(f"{n},{nEvents}\n")
   print(f"Wrote {len(nMuons_list)} job(s) to '{output_txt_file}':")

if not nEvents_constant:
   with open(output_txt_file, 'w') as txt_file:
      for i in range(len(nMuons_list)):
         txt_file.write(f"{nMuons_list[i]},{nEvents_list[i]}\n")
   print(f"Wrote {len(nMuons_list)} job(s) to '{output_txt_file}':")



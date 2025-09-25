import os
import shutil

# Linux source folder
source_folder = "/home/ashis/cms_open_data_python"

# Windows Desktop folder path (mounted in WSL)
destination_folder = "/mnt/c/Users/ashis/OneDrive/Desktop/WW_background"
os.makedirs(destination_folder, exist_ok=True)

# Copy tempsignalfinder1.root to tempsignalfinder60.root
for i in range(1, 88):
    filename = f"tempsignalfinder{i}.root"
    src = os.path.join(source_folder, filename)
    dest = os.path.join(destination_folder, filename)

    try:
        print(f"Copying {src} -> {dest}")
        shutil.copyfile(src, dest)
    except Exception as e:
        print(f"❌ Failed to copy {filename}: {e}")

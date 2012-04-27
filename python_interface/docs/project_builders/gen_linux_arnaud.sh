cd ~/diyabc/python_interface/docs/project_builders/
./linux_generation.sh ./pyinstaller_1743/pyinstaller.py /tmp/outtmp ../../diyabc.py
zip -r diyabc_linux.zip /tmp/outtmp/diy*

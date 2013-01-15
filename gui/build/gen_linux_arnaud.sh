rm -rf /tmp/outtmp
cd ~/diyabc/python_interface/docs/project_builders/
./linux_generation.sh ./pyinstaller_1743/pyinstaller.py /tmp/outtmp ../../diyabc.py
rm -rf  ~/diyabc_linux.zip
cd /tmp/outtmp
zip -r ~/diyabc/python_interface/docs/project_builders/diyabc_linux.zip diy*
mv ~/diyabc/python_interface/docs/project_builders/diyabc_linux.zip ~/
notify-send "Le zip a été généré dans /home/estoup"

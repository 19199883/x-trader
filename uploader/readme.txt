uploader:用于云服务器从生产服务器现在日志和行情数据

压缩
tar -czvf /path/to/file.tar.gz file

解压
tar -xzvf /path/to/file.tar.gz /path/to

加密压缩
tar -czvf - file | openssl des3 -salt -k password -out /path/to/file.tar.gz

解密解压
openssl des3 -d -k password -salt -in /path/to/file.tar.gz | tar xzf -

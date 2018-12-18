decryptor:对加密脚本进行解密并执行。脚本加密：防止在云服务器的脚本被外人窃取，从而访问生产环境

压缩
tar -czvf /path/to/file.tar.gz file

解压
tar -xzvf /path/to/file.tar.gz /path/to

加密压缩
tar -czvf - file | openssl des3 -salt -k password -out /path/to/file.tar.gz

解密解压
openssl des3 -d -k password -salt -in /path/to/file.tar.gz | tar xzf -

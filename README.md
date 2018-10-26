# paralegrep
Thread implementation for search files and look for pattern strings 

# BUILD
Antes de realizar a compilação, certifique-se que o caminho para a thread despachante existe
Para compilar a aplicação deve-se usar o seguinte comando:

gcc -g -Wall -pthread nome_arquivo.c -lpthread -o nome_programa

ou 

gcc -o thread thread_despachante.c -lpthread -D_REENTRANT -DLinux

# RUN 

./paralegrep "palavra a procurar"
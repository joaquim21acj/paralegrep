int main()
{
    int flag_despachante, flag_operarias, flag_ranking;

    /*A função terá que receber da thread despachante o 
    arquivo para que a thread operária tenha mais controle*/
    arquivo *a = (arquivo *) malloc(sizeof(arquivo));
    a->arquivo = fileset;
    a->n_vezes=0;


    printf("\nA criar uma nova thread\n");
    //printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag = pthread_create(&t_o.t_o, NULL, trata_thread(a), NULL);

    if (flag_despachante != 0){
        printf("\nErro na criação da thread despachante thread\n");
    }
    if (flag_operarias !=0){
        printf("\nErro na criação da thread operaria thread\n");
    }
    else (flag_ranking != 0){
        printf("\nErro na criação da thread ranking thread\n");
    }
    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}
int main()
{
    int flag;
    char diretorio_prog[FILENAME_MAX];
    GetCurrentDir(diretorio_prog, FILENAME_MAX );

    /*A função terá que receber da thread despachante o 
    arquivo para que a thread operária tenha mais controle*/
    arquivo *a = (arquivo *) malloc(sizeof(arquivo));
    a->arquivo = fileset;
    a->n_vezes=0;


    printf("\nA criar uma nova thread\n");
    //printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag = pthread_create(&t_o.t_o, NULL, trata_thread(a), NULL);

    if (flag != 0)
        printf("\nErro na criação da thread despachante thread\n");

    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}

int main()
{
    int flag;
    char diretorio_prog[FILENAME_MAX];
    GetCurrentDir(diretorio_prog, FILENAME_MAX );

    printf("\nA criar uma nova thread\n");
    printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag = pthread_create(&t_d.t_d, NULL, trata_thread(fileset), NULL);

    if (flag != 0)
        printf("\nErro na criação da thread despachante thread\n");

    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}
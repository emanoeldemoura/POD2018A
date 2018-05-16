#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<fstream>
#include<string>
#include<cstring>

//DEFINIMOS QUE O TAMANHO DA MEMORIA Eh 100
#define MEM 100

using namespace std;

// ESRTRUTURA
struct arquivo{
   fstream saida;
   int pos, max;
   string *buffer;
};

//PREENCHE BUFFER
void preenche_buffer(struct arquivo* arq,int tamanho_arq){
     int i;
     arq->pos = 0;
     arq->max = 0;

     for(i=0;i<tamanho_arq;i++){
        // se diferente do fim do arquivo
        if(arq->saida.good()){
           getline(arq->saida,arq->buffer[arq->max]);
           arq->max++;
        }else{
           arq->saida.close();
           break;
        }
     }
}

// PROCURA MENOR PALVAVRA
int procura_menor_palavra(struct arquivo *arq, int n_arquivos,int tamanho_arq,string *menor_palavra){
   int i, index = -1;
   //procura menor palavra na primeira posicao de cada buffer
   for(i=0;i<n_arquivos;i++){
       if(arq[i].pos < arq[i].max){
          if(index == -1){
             index = i;
          }else{
               if(strcmp(arq[i].buffer[arq[i].pos].c_str(),arq[index].buffer[arq[index].pos].c_str())<0){
                  index = i;
               }
          }
       }
   }
   // se achou menor, atualiza posicao do buffer. Enche se estiver vazio.
   if(index!=-1){
        *menor_palavra = arq[index].buffer[arq[index].pos];
        arq[index].pos++;
        if(arq[index].pos == arq[index].max)
            preenche_buffer(&arq[index],tamanho_arq);
         return 1;
   }else
         return 0;
}

//SALVA ARQUIVO
void salva_arquivo(char* entrada, string *vet_string,int total,int muda_linha){
    int i;
    fstream arquivo_salvo;
    //abre arquivo
    arquivo_salvo.open(entrada, std::fstream::app);
    for(i=0;i<total-1;i++){
       arquivo_salvo << vet_string[i] << endl;
    }

    if(muda_linha==0){
       arquivo_salvo << vet_string[total-1];
    }else{
      arquivo_salvo << vet_string[total-1] << endl;
    }
    //fecha arquivo
    arquivo_salvo.close();
}

// MERGE SORT
void realizar_merge(string entrada, int n_arquivos, int tamanho_arq){
     char temp[20];
     int i, quantidade_buffer = 0;
     string *buffer = new string[tamanho_arq];
     string menor_palavra;
     struct arquivo *arq = new arquivo[n_arquivos];

     for(i=0;i<n_arquivos;i++){
         sprintf(temp,"Temp%d.txt",i+1);
         arq[i].saida.open(temp);
         arq[i].pos = 0;
         arq[i].max = 0;
         arq[i].buffer = new string[tamanho_arq];
         preenche_buffer(&arq[i],tamanho_arq);
     }

     //Se existe menor elemento.
     while(procura_menor_palavra(arq,n_arquivos,tamanho_arq,&menor_palavra)==1){
          buffer[quantidade_buffer] = menor_palavra;
          quantidade_buffer++;

          //Salvar se Buffer cheio.
          if(quantidade_buffer==tamanho_arq){
             //Insere no buffer de saida.
             //salva_arquivo("saida.txt",buffer,tamanho_arq,1);
             salva_arquivo(const_cast<char*>("saida.txt"),buffer,tamanho_arq,1);
             quantidade_buffer = 0;
          }
     }

     if(quantidade_buffer>0){
         //salva_arquivo("saida.txt",buffer,quantidade_buffer,1);
         salva_arquivo(temp,buffer,quantidade_buffer,1);
     }
}

// CRIA ARQUIVO TEMPORARIO
int criar_arq_temporario(string entrada){
    char temp[20];
    string palavras[MEM];
    int contador=0, total=0;
    string linha;
    // string contendo os caracteres especias a serem desconciderados durante a ordenacao
    string separador(" \t\n\r,.!?:;\"'()-_+=´^[]{}`*");
    ifstream arquivo;

    arquivo.open(entrada.c_str());
    while(arquivo.good()){
        while(getline(arquivo,linha)){
           size_t offset =0, fim;
           do{
              offset = linha.find_first_not_of(separador,offset);
              if(offset==string::npos){
                 break;
              }
              fim = linha.find_first_of(separador,offset);
              string palavra(linha.substr(offset,fim-offset));
              transform(palavra.begin(),palavra.end(),palavra.begin(), ::tolower);
              palavras[total] = palavra;
              total++;
              // se total igual ao tamanho da memoria , ordena o arquivo e salva
              if(total==MEM){
                 contador++;
                 sprintf(temp,"Temp%d.txt",contador);
                 std::sort(palavras,palavras+total);
                 salva_arquivo(temp,palavras,total,0);
                 // zera total para iniciar um novo processo
                 total=0;
              }
              offset=fim+1;
           }while(fim!=string::npos);
        }
    }
    // sobrou elementos
    if(total>0){
        contador++;
        sprintf(temp,"Temp%d.txt",contador);
        std::sort(palavras,palavras+total);
        salva_arquivo(temp,palavras,total,0);
    }
    arquivo.close();
    return contador;
}

// MERGE SORT EXTERNO
void merge_sort_externo(string entrada){
   char temp[20];
   int n_arquivos = criar_arq_temporario(entrada), i,k;
   k = MEM/(n_arquivos+1);
   //ordenar no arquivo original
   realizar_merge(entrada,n_arquivos,k);

   // apagar arquivos temporarios
   for(i=0;i<n_arquivos;i++){
      sprintf(temp,"Temp%d.txt",i+1);
      remove(temp);
   }
}

//PRICIPAL
int main(){
   // remove o arquivo txt saida ja criado para nao inserir o novo ordenado no mesmo.
   remove("saida.txt");

   string texto;

   cout <<"INFORME O NOME DO ARQUIVO TEXTO A SER ORDENADO (texto.txt): ";
   getline(std::cin,texto);

   // chama funcao merge sort externo
   cout <<"\n\n\n\n\n\n\t\t\t\tAGUARDE...\n\n\n";
   merge_sort_externo(texto);
   system("cls");
   cout <<"\n\n\n\n\n\n\t\t\tARQUIVO ORDENADO COM SUCESSO!\n\n\n";
}

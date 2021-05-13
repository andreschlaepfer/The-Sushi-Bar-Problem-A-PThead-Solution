//
//  main.c
//  TrabalhoSO2
//
//  Created by Andre Schlaepfer on 25/02/21.
//
//  7.1 THE SUSHI BAR PROBLEM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//Para este problema, cada thread é um cliente.

char lugares[5][10]; //tem funcao ilustrativa apenas
int vagas = 5;       //qnt de lugares vazios no bar

//0, quando o numero de vagas ainda nao chegou a 0. Quando o numero de vagas chega a 0, significa que estao comendo juntos e "comendoJunto" passa a ser 1. Volta a ser 0 apenas quando o numero de vagas volta a ser 5
int comendoJunto = 0;

int clientesSatisfeitos = 0; //controle

//imprime o status do sushibar
//funcao ilustrativa do problema
void mostrarLugares()
{
  printf("|%s| |%s| |%s| |%s| |%s| \n", lugares[0], lugares[1], lugares[2], lugares[3], lugares[4]);
}

//ocupa uma vaga
void sentar()
{
  pthread_mutex_lock(&mutex);
  while (comendoJunto == 1)
  {

    pthread_cond_wait(&cond, &mutex);
  }

  for (int i = 0; i < 5; i++)
  {
    if (strcmp(lugares[i], "vazio") == 0)
    {
      strcpy(lugares[i], "ocupado");
      vagas--;
      if (vagas == 0)
      {
        comendoJunto = 1; //significa que estao comendo juntos
      }
      mostrarLugares();

      break;
    }
  }
  pthread_mutex_unlock(&mutex);
}

//libera uma vaga
void levantar()
{
  pthread_mutex_lock(&mutex);
  for (int i = 0; i < 5; i++)
  {
    if (strcmp(lugares[i], "vazio") != 0)
    {
      strcpy(lugares[i], "vazio");
      mostrarLugares();
      vagas++;
      clientesSatisfeitos++;
      printf("%d clientes satisfeitos!\n", clientesSatisfeitos);
      if (vagas == 5)
      {
        comendoJunto = 0; //entrada liberada para novas threads
        pthread_cond_broadcast(&cond);
      }
      break;
    }
  }
  pthread_mutex_unlock(&mutex);
}

//funcao que cada thread vai executar
void *entrarNoSushiBar(void *arg)
{

  sentar();

  //aqui defini um tempo que cada thread passara "comendo" no sushi bar
  //para fim de simplicidade, cada thread passa o exato mesmo tempo comendo,
  //mas essa decisao nao afeta o funcionamento do codigo
  sleep(10);

  levantar();

  pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{

  //pedindo numero de clientes
  int n_clientes;
  printf("Defina o número de clientes do Sushi Bar: ");
  scanf("%d", &n_clientes);
  printf("%d clientes no total! \n", n_clientes);

  //string copy
  strcpy(lugares[0], "vazio");
  strcpy(lugares[1], "vazio");
  strcpy(lugares[2], "vazio");
  strcpy(lugares[3], "vazio");
  strcpy(lugares[4], "vazio");
  mostrarLugares();

  pthread_t th[n_clientes];

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  for (int i = 0; i < n_clientes; i++)
  {

    //criando threads
    pthread_create(th + i, NULL, entrarNoSushiBar, NULL);
    //printf("Thread %d foi criada \n", i);
  }

  for (int i = 0; i < n_clientes; i++)
  {
    //esperando as threads
    pthread_join(th[i], NULL);
    // printf("Thread %d terminou\n", i);
  }

  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);
  return 0;
}

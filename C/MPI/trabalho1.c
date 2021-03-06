/*
*  <Dif Fim do Aluno MPI> is a  <Software para calcular o problema da diferença finita utilizando a biblioteca MPI >
*  Copyright (C) 2010 Gustavo Liberatti <liberatti.gustvo@gmail.com>
*		      Rodicrisller Rodrigues <rodi_67@gmail.com>
		      Guilherme W. dos Santos Lopes <gwslopes@gmail.com> 			
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*/
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#define MAX 256
#define TAG 100

	int rank, size, tempototal;
	char buffer[MAX];
	MPI_Status status;
	float *vector;
	int t=-1;
	float valor = 0;

int main (int argc, char **argv){

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 	MPI_Comm comm;
	
	

	if(rank == 0 && t == -1){
		printf("Entre com o valor do tempo total de execução \n");		
		fflush(stdout);	
		scanf ("%d", &tempototal);	
		int i;
		for (i = 1; i < size; i++) {
			
			MPI_Send(&tempototal, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
		}
	}
	
	
			if (rank != 0){

				MPI_Recv(&tempototal, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
			}
			
			t=1;

			while (t<=tempototal){
				

				//Envia Mensagens				
				if (rank == 0) {
					valor = 100;
					//Como é o unico valor definido, envia 100 para a proxima posição
					MPI_Send(&valor, 1, MPI_FLOAT, rank+1, t, MPI_COMM_WORLD);
				}
				else{
					if (rank == size-1) {
						//Se este for o Ultimo processo este envia seu valor para a posição anterior a ele mas não envia nenhuma mensagem 							ao posterior já que este não existe
						MPI_Send(&valor, 1, MPI_FLOAT, rank-1, t, MPI_COMM_WORLD);

					}
					else {
						//Envia o valor o para o anterior
						MPI_Send(&valor, 1, MPI_FLOAT, rank-1, t, MPI_COMM_WORLD);
						//Envia o valor para o proximo
						MPI_Send(&valor, 1, MPI_FLOAT, rank+1, t, MPI_COMM_WORLD);
;
					}
				}

				float anterior, posterior;
				
				//Recebe mensagens
				if (rank == 0){	
					MPI_Recv(&posterior, 1, MPI_FLOAT, rank+1, t, MPI_COMM_WORLD, &status);
				}
				else{
					if(rank == size-1) {
						//Conforme restrição pega o valor do anterior e tem o peso 3
						MPI_Recv(&anterior, 1, MPI_FLOAT, rank-1, t, MPI_COMM_WORLD, &status);
						valor = (anterior + 3*valor)/4;

					} else {
						//Conforme o exercicio, pega o valor do anterior e da posterior, soma com 2 vezes o valor atual e divide por 4
						MPI_Recv(&anterior, 1, MPI_FLOAT, rank-1, t, MPI_COMM_WORLD, &status);
						MPI_Recv(&posterior, 1, MPI_FLOAT, rank+1, t, MPI_COMM_WORLD, &status);
						valor = (anterior + 2*valor + posterior)/4;
					}
				}
				t++;
			}

			
				vector = (float *)malloc(size * sizeof(float));

				if(rank != 0){
					//Cada processo manda o seu valor para o processo 0;
					MPI_Send(&valor, 1, MPI_FLOAT, 0 , t, MPI_COMM_WORLD);
				}

				if (rank == 0){

					vector[0] = 100;
					printf("A posição 1 teve o valor carregado: %f \n", vector[0]);
					fflush(stdout);				
					int i;
					//Para cada interação ele recebe a resposta de um processo e coloca ele no vetor
					for (i = 1; i < size; i++){
						MPI_Recv(&valor, 1, MPI_FLOAT, i , t, MPI_COMM_WORLD, &status);
						vector [i] = valor;
						printf("A posição %d teve o valor carregado: %f \n", i+1, vector[i]);
						fflush(stdout);
					}
				}
	MPI_Finalize();
	return 0;

}

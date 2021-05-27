/* Campo Minado em C
 * Programa com fins educacionais para a disciplina de programação, fornecido como exemplo para 
 * os alunos de Ciência da Computação das Faculdades Integradas de Caratinga (Doctum)
 * Autor: Jacson Rodriges Correia-Silva <jacsonrcsilva@gmail.com>
 * Para compilar: gcc -o campoMinado.bin campoMinado.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <time.h>
typedef enum { false, true } boolean;

#define Aviso(msg)   printf("Aviso:: " msg "\n");
#define Cuidado(msg) printf("CUIDADO:: " msg "\n");

#define charBomba '*'
#define charBandeira 'B'
#define charDuvida '?'
#define charVisitado '0'
#define charNaoVisitado '.'

enum enumConteudo { comBomba, semBomba };
enum enumEstado   { bandeira, duvida, visitado, naoVisitado };
enum enumConteudoTabuleiro { conteudo, estado, normal, minas };
enum enumUsuarioSituacao   { perdeu, ganhou, jogando };

/// Conteúdo da posição
typedef enum enumConteudo conteudoPos;
/// Estado da posição
typedef enum enumEstado   estadoPos;

/// Conteúdo do tabuleiro
typedef enum enumConteudoTabuleiro conteudoTabuleiro;

/// Indicação do resultado da jogada do usuário
typedef enum enumUsuarioSituacao usuarioSituacao;

typedef struct posTabuleiro {
	/// Estado: qual o estado da posição do tabuleiro
	conteudoPos conteudo;
	estadoPos   estado;
	int   qntBombas;
} posTabuleiro;

typedef struct tabuleiro {
	posTabuleiro **posicoes;
	int linhas;
	int colunas;
	int bombas;
	int bombasMarcadas;
} tabuleiro;

boolean possuiBomba(posTabuleiro pos)
{
	return (pos.conteudo == comBomba)?true:false;
}

char conteudoToChar(posTabuleiro pos)
{
	// C: com bomba
	// S: sem bomba
	return (pos.conteudo == comBomba)?charBomba:charNaoVisitado;
}

char estadoToChar(posTabuleiro pos)
{
	// B: bandeira
	// ?: duvida
	// V: visitado
	// N: nao visitado
	return (pos.estado == bandeira)?charBandeira:
		 (pos.estado == duvida)?charDuvida:
		 (pos.estado == visitado)?charVisitado:
		                          charNaoVisitado;
}

char normalToChar(posTabuleiro pos)
{
	if (pos.estado == visitado)
	{
		if ( possuiBomba(pos) )
			return charBomba;
		else
			return (pos.qntBombas==0)?
				charVisitado:
				pos.qntBombas+48;
	}
	else return estadoToChar(pos);
}

char obtemCharPosicao(posTabuleiro pos, conteudoTabuleiro cont)
{
	switch (cont) {
		case conteudo:
			return conteudoToChar( pos );
			break;
		case estado:
			return estadoToChar( pos );
			break;
		case normal:
			return normalToChar( pos );
			break;
		case minas:
			if ( pos.conteudo == comBomba )
				return charBomba;
			else    
				return (pos.qntBombas==0)?
					charVisitado:
					pos.qntBombas+48;
			break;
	}
}

void imprimeTabuleiro(tabuleiro *tab, conteudoTabuleiro cont, boolean rotulo)
{
	int lin, col, posicao;
	char aux;
	
	posicao = (tab->colunas*4)/2+6;
	printf("\n%*s\n",posicao, "Campo Minado");
	printf("    Bombas: %3d %*s%3d\n",
			tab->bombas,
			tab->colunas*4-15,
			"Bombas Marcadas:",
		       	tab->bombasMarcadas );
	printf("\n");

	for ( lin=0 ; lin < tab->linhas ; lin++ )
	{
		if (rotulo) {
			if(lin == 0) {
				printf("L/C:");
				for (col=0 ; col < tab-> colunas; col++)
				       	printf("%3d ",col+1);
				printf("\n");

				printf("    ");
				for (col=0 ; col < tab-> colunas; col++)
				       	printf("----");
				printf("\n");
			}
			printf("%3d:", lin+1);
		}

		for ( col=0 ; col < tab->colunas ; col++ )
			printf("  %c ", obtemCharPosicao(tab->posicoes[lin][col], cont) );
		printf("\n");
	}
}

/**
 * @fn AlocaMemoriaTabuleiro(tabuleiro *tab)
 * @brief Aloca Memória para um Tabuleiro
 * Aloca memória para um tabuleiro já definindo suas
 * posições como: Sem Bomba, Não visitado e com 0 bombas
 */
tabuleiro alocaMemoriaTabuleiro(tabuleiro *tab)
{
	int i, j;
	tab->posicoes = (posTabuleiro**) calloc( tab->linhas, sizeof(posTabuleiro*) );
	for ( i=0 ; i < tab->linhas ; i++ )
	{
		tab->posicoes[i] = (posTabuleiro*) calloc( tab->colunas, sizeof(posTabuleiro) );
		for ( j=0 ; j < tab->colunas ; j++ )
		{
			tab->posicoes[i][j].conteudo  = semBomba;
			tab->posicoes[i][j].estado    = naoVisitado;
			tab->posicoes[i][j].qntBombas = 0;
		}
	}
}

void liberaMemoriaTabuleiro(tabuleiro *tab)
{
	int i, j;
	for ( i=0 ; i < tab->linhas ; i++ )
		free( tab->posicoes[i] );

	free( tab->posicoes );
}

boolean verificaPosicaoTabValida(tabuleiro *tab, int linha, int coluna)
{
	/// Verifica se a matriz existe
	if ( tab == NULL )
		return false;

	/// Verifica então se a posição existe
	if ( linha  < 0 || linha  >= (tab->linhas) ||
	     coluna < 0 || coluna >= (tab->colunas)
	   )  return false;
	else  return true;
}

boolean adicionaNumBombas(tabuleiro *tab, int linha, int coluna)
{
	if (! verificaPosicaoTabValida( tab, linha, coluna ) )
		return false;

	tab->posicoes[linha][coluna].qntBombas ++;
	return true;
}

boolean insereContagemBombas(tabuleiro *tab, int linha, int coluna)
{
	boolean retorno = true;

	if (! verificaPosicaoTabValida( tab, linha, coluna ) )
		return false;

	retorno &= adicionaNumBombas( tab, linha-1, coluna-1 );
	retorno &= adicionaNumBombas( tab, linha-1, coluna   );
	retorno &= adicionaNumBombas( tab, linha-1, coluna+1 );
	retorno &= adicionaNumBombas( tab, linha  , coluna-1 );
	retorno &= adicionaNumBombas( tab, linha  , coluna+1 );
	retorno &= adicionaNumBombas( tab, linha+1, coluna-1 );
	retorno &= adicionaNumBombas( tab, linha+1, coluna   );
	retorno &= adicionaNumBombas( tab, linha+1, coluna+1 );

	return retorno;
}

boolean distribuiMinas(tabuleiro *tab)
{
	if (tab == NULL) return false;
	else
	{
		int cont, linha, coluna;
		time_t semente = time( NULL );
		srand(semente);
		
		for (cont = 0; cont < tab->bombas ; cont++)
		{
			do{
				linha  = rand() % tab->linhas;
				coluna = rand() % tab->colunas;
			} while( possuiBomba( tab->posicoes[linha][coluna] ) );
			tab->posicoes[linha][coluna].conteudo = comBomba;
			insereContagemBombas(tab, linha, coluna);
		}

		return true;
	}
}

tabuleiro criaTabuleiro(int linhas, int colunas, int qntBombas)
{
	tabuleiro tab;

	tab.linhas  = linhas;
	tab.colunas = colunas;
	tab.bombas  = qntBombas;
	tab.bombasMarcadas = 0;

	alocaMemoriaTabuleiro(&tab);
	if ( ! distribuiMinas(&tab) )
		Cuidado("Não foi possível distribuir bombas");

	return tab;
}

boolean usuarioMarcaPosicao(tabuleiro *tab, int linha, int coluna, estadoPos est)
{
	/// Se é posição inválida, já sai
	if (! verificaPosicaoTabValida( tab, linha, coluna ) )
		return false;
	/// Se é posição já visitada, sai também
	if ( tab->posicoes[linha][coluna].estado == visitado ||
	     (tab->posicoes[linha][coluna].estado == bandeira && est != bandeira)
	   )
		return true;

	// Se o pedido for para marcar bandeira e a posição já for bandeira,
	//  o número de bombas marcadas é incrementado
	if ( est == bandeira && tab->posicoes[linha][coluna].estado != bandeira )
			tab->bombasMarcadas++;

	// Se a posição já é bandeira, e o pedido for qualquer um,
	//  o número de bombas marcadas é decrementado
	if ( tab->posicoes[linha][coluna].estado == bandeira )
		tab->bombasMarcadas--;

	/// Se a posição já for bandeira e o pedido for bandeira, desmarca,
	//     senão marca normalmente
	if ( est == bandeira && tab->posicoes[linha][coluna].estado == bandeira)
		tab->posicoes[linha][coluna].estado = naoVisitado;
	else
		tab->posicoes[linha][coluna].estado = est;

	// Se estiver marcando e a posição for 0, abre as posições adjacentes
	if ( tab->posicoes[linha][coluna].qntBombas == 0 && est == visitado)
	{
		usuarioMarcaPosicao( tab, linha-1, coluna-1 , visitado);
		usuarioMarcaPosicao( tab, linha-1, coluna   , visitado);
		usuarioMarcaPosicao( tab, linha-1, coluna+1 , visitado);
		usuarioMarcaPosicao( tab, linha  , coluna-1 , visitado);
		usuarioMarcaPosicao( tab, linha  , coluna+1 , visitado);
		usuarioMarcaPosicao( tab, linha+1, coluna-1 , visitado);
		usuarioMarcaPosicao( tab, linha+1, coluna   , visitado);
		usuarioMarcaPosicao( tab, linha+1, coluna+1 , visitado);
	}

	return true;
}

usuarioSituacao usuarioVerificaSituacao(tabuleiro *tab)
{
	int lin, col;
	int bombas=0, posVisitadas=0;
	for ( lin=0 ; lin < tab->linhas ; lin++ )
		for ( col=0 ; col < tab->colunas ; col++ )
			if (tab->posicoes[lin][col].estado == visitado)
				if (tab->posicoes[lin][col].conteudo == comBomba)
					return perdeu;
				else posVisitadas++;
	if ( posVisitadas == (tab->linhas*tab->colunas - tab->bombas) )
		return ganhou;
	else	return jogando;
}

void mensagemGanhou()
{
	printf("\n\tParabéns, você é um vencedor!!\n");
}

void mensagemPerdeu()
{
	printf("\n\tQue pena, você perdeu... tente novamente mais tarde!\n");
}

void limparTela()
{
	if ( system("clear") ) system("cls");
}

void pausar()
{
	__fpurge(stdin);
	printf("\nPressione ENTER para continuar...");

	scanf("%*c");
	//Se não funcionar no Windows, troque o scanf pela Linha abaixo:
	// if ( system("read") ) system("pause");
}

int main(int argc, char *argv[])
{
	tabuleiro Tabuleiro;
	int linha, coluna,
        qnt_linhas=10, qnt_colunas=10, qnt_bombas=10;
	char opt;
    if (argc == 4)
    {
        qnt_linhas  = atoi(argv[1]);
        qnt_colunas = atoi(argv[2]);
        qnt_bombas  = atoi(argv[3]);
    }
    Tabuleiro = criaTabuleiro(qnt_linhas, qnt_colunas, qnt_bombas);
	imprimeTabuleiro(&Tabuleiro, minas, true);
	pausar();
	do {
		limparTela();
		imprimeTabuleiro(&Tabuleiro, normal, true);
		printf("\nDigite a posição para jogar (linha coluna): ");
		scanf("%d %d", &linha, &coluna);
		
		opt='n';
		printf("Deseja [V|v]isitar, marcar com [D|d]úvida, ou com [B|b]andeira: ");
		while (opt != 'v' && opt != 'V' && opt != 'd' && opt != 'D' && opt != 'b' && opt != 'B')
		{
			scanf("%c",&opt);
		}
		switch (opt) {
			case 'v':
			case 'V':
				usuarioMarcaPosicao(&Tabuleiro,linha-1,coluna-1,visitado);
				break;
			case 'd':
			case 'D':
				usuarioMarcaPosicao(&Tabuleiro,linha-1,coluna-1,duvida);
				break;
			case 'b':
			case 'B':
				usuarioMarcaPosicao(&Tabuleiro,linha-1,coluna-1,bandeira);
				break;
			default:
				Aviso("Opção Inválida!");
				pausar();
		}
	} while( usuarioVerificaSituacao(&Tabuleiro) == jogando);

	limparTela();
	imprimeTabuleiro(&Tabuleiro, minas, true);
	switch ( usuarioVerificaSituacao(&Tabuleiro) ) {
		case ganhou:
			mensagemGanhou();
			break;
		case perdeu:
			mensagemPerdeu();
			break;
	}
	liberaMemoriaTabuleiro(&Tabuleiro);
	pausar();
}

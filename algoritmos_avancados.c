#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

#define TAMANHO_TABELA 10

// ======================= ESTRUTURAS =======================

// Árvore de salas
typedef struct No {
    char valor[50];
    char pista[50];
    struct No* esquerda;
    struct No* direita;
} No;

// Árvore de pistas coletadas
typedef struct NoBST {
    char pista[50];
    struct NoBST* esquerda;
    struct NoBST* direita;
} NoBST;

// Lista encadeada de pistas de cada suspeito
typedef struct PistaNode {
    char pista[50];
    struct PistaNode* proximo;
} PistaNode;

// Estrutura da tabela hash de suspeitos
typedef struct Nodo {
    char suspeito[50];
    PistaNode* pistas;
    struct Nodo* proximo;
} Nodo;

Nodo* tabela_suspeitos[TAMANHO_TABELA];

// ======================= CRIAÇÃO DAS ESTRUTURAS =======================

No* criarNo(const char* valor, const char* pista) {
    No* novo = (No*)malloc(sizeof(No));
    strcpy(novo->valor, valor);
    strcpy(novo->pista, pista);
    novo->esquerda = novo->direita = NULL;
    return novo;
}

NoBST* criarNoBST(const char* pista) {
    NoBST* nova = (NoBST*)malloc(sizeof(NoBST));
    strcpy(nova->pista, pista);
    nova->esquerda = nova->direita = NULL;
    return nova;
}

PistaNode* criar_pista_node(const char* pista) {
    PistaNode* nova = (PistaNode*)malloc(sizeof(PistaNode));
    strcpy(nova->pista, pista);
    nova->proximo = NULL;
    return nova;
}

// ======================= FUNÇÕES DE PISTAS (BST) =======================

NoBST* criarPista(NoBST* raiz, const char* pista) {
    if (raiz == NULL)
        return criarNoBST(pista);

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = criarPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = criarPista(raiz->direita, pista);

    return raiz;
}

void listarTodasPistas(NoBST* raiz) {
    if (raiz != NULL) {
        listarTodasPistas(raiz->esquerda);
        printf("• %s\n", raiz->pista);
        listarTodasPistas(raiz->direita);
    }
}

// ======================= ÁRVORE DE SALAS =======================

No* CriarSala(No* raiz, const char* valor, const char* pista) {
    if (raiz == NULL)
        return criarNo(valor, pista);

    if (strcmp(valor, raiz->valor) < 0)
        raiz->esquerda = CriarSala(raiz->esquerda, valor, pista);
    else
        raiz->direita = CriarSala(raiz->direita, valor, pista);

    return raiz;
}

// ======================= TABELA HASH DE SUSPEITOS =======================

int funcao_hash(const char* chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++)
        soma += chave[i];
    return soma % TAMANHO_TABELA;
}

// Inserir suspeito e suas pistas (múltiplas)
void inserir_suspeito(const char* suspeito, const char* pista) {
    int indice = funcao_hash(suspeito);
    Nodo* atual = tabela_suspeitos[indice];

    while (atual != NULL) {
        if (strcmp(atual->suspeito, suspeito) == 0) {
            PistaNode* nova = criar_pista_node(pista);
            nova->proximo = atual->pistas;
            atual->pistas = nova;
            return;
        }
        atual = atual->proximo;
    }

    Nodo* novo = (Nodo*)malloc(sizeof(Nodo));
    strcpy(novo->suspeito, suspeito);
    novo->proximo = tabela_suspeitos[indice];
    novo->pistas = criar_pista_node(pista);
    tabela_suspeitos[indice] = novo;
}

Nodo* encontrar_suspeito(const char* suspeito) {
    int indice = funcao_hash(suspeito);
    Nodo* atual = tabela_suspeitos[indice];
    while (atual != NULL) {
        if (strcmp(atual->suspeito, suspeito) == 0)
            return atual;
        atual = atual->proximo;
    }
    return NULL;
}

// Buscar suspeitos por uma pista
void buscar_suspeito_por_pista(const char* pista) {
    bool encontrada = false;

    for (int i = 0; i < TAMANHO_TABELA; i++) {
        Nodo* atual = tabela_suspeitos[i];
        while (atual != NULL) {
            PistaNode* p = atual->pistas;
            while (p != NULL) {
                if (strcmp(p->pista, pista) == 0) {
                    printf("\n• A pista '%s' está associada a: %s\n", pista, atual->suspeito);
                    encontrada = true;
                }
                p = p->proximo;
            }
            atual = atual->proximo;
        }
    }

    if (!encontrada)
        printf("\nNenhum suspeito foi encontrado para a pista '%s'.\n", pista);
}

// Conta quantas pistas associadas a um suspeito foram coletadas
int contar_pistas_do_suspeito(NoBST* pistasColetadas, PistaNode* pistasSuspeito) {
    if (pistasSuspeito == NULL || pistasColetadas == NULL)
        return 0;

    int total = 0;
    while (pistasSuspeito != NULL) {
        if (strcmp(pistasColetadas->pista, pistasSuspeito->pista) == 0)
            total++;

        total += contar_pistas_do_suspeito(pistasColetadas->esquerda, pistasSuspeito);
        total += contar_pistas_do_suspeito(pistasColetadas->direita, pistasSuspeito);
        pistasSuspeito = pistasSuspeito->proximo;
    }
    return total;
}

// ======================= MOVIMENTO DO JOGADOR =======================

No* PortaEsquerda(No* raiz, NoBST** pistasColetadas) {
    if (raiz->esquerda == NULL) {
        printf("Não há porta à esquerda de %s.\n", raiz->valor);
        return raiz;
    }

    raiz = raiz->esquerda;
    printf("Agora você está em: %s\n", raiz->valor);

    if (strcmp(raiz->pista, " ") != 0 && strlen(raiz->pista) > 0) {
        printf("Você encontrou uma pista: %s!\n", raiz->pista);
        *pistasColetadas = criarPista(*pistasColetadas, raiz->pista);
        strcpy(raiz->pista, " ");
    }

    return raiz;
}

No* PortaDireita(No* raiz, NoBST** pistasColetadas) {
    if (raiz->direita == NULL) {
        printf("Não há porta à direita de %s.\n", raiz->valor);
        return raiz;
    }

    raiz = raiz->direita;
    printf("Agora você está em: %s\n", raiz->valor);

    if (strcmp(raiz->pista, " ") != 0 && strlen(raiz->pista) > 0) {
        printf("Você encontrou uma pista: %s!\n", raiz->pista);
        *pistasColetadas = criarPista(*pistasColetadas, raiz->pista);
        strcpy(raiz->pista, " ");
    }

    return raiz;
}

// ======================= UTILITÁRIOS =======================

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ======================= MAIN =======================

int main() {
    setlocale(LC_ALL, "Portuguese");

    No* raiz = NULL;
    NoBST* pistasColetadas = NULL;

    // RAIZ
    raiz = CriarSala(raiz, "Hall de Entrada", " "); // sem pista

    // ======================= CAMINHOS COM SUSPEITOS =======================

    // CAMINHO 1 – SUSPEITO: Dr. Silva (2+ pistas)
    raiz = CriarSala(raiz, "Laboratório Secreto", "Frasco quebrado");
    raiz = CriarSala(raiz, "Laboratório Antigo", "Anotação misteriosa");
    raiz = CriarSala(raiz, "Banheiro Principal", "Toalha molhada"); // terceira pista opcional

    // CAMINHO 2 – SUSPEITO: Marcos
    raiz = CriarSala(raiz, "Cozinha", "Faca afiada");
    raiz = CriarSala(raiz, "Sala de Jantar", "Guardanapo manchado");
    raiz = CriarSala(raiz, "Piscina", "Roupa molhada");

    // CAMINHO 3 – SUSPEITO: Sofia
    raiz = CriarSala(raiz, "Ateliê", "Pincel sujo de tinta vermelha");
    raiz = CriarSala(raiz, "Galeria de Arte", "Quadro rasgado");
    raiz = CriarSala(raiz, "Closet", "Perfume derramado");

    // CAMINHO 4 – SUSPEITO: Pedro (Mordomo)
    raiz = CriarSala(raiz, "Quintal", "Chave dourada");
    raiz = CriarSala(raiz, "Sala do Cofre", "Cofre forçado");
    raiz = CriarSala(raiz, "Guarda-Volumes", "Luva de couro");

    // CAMINHO 5 – SUSPEITO: Otávio
    raiz = CriarSala(raiz, "Escritório", "Documento desaparecido");
    raiz = CriarSala(raiz, "Sala de Controle", "Disco rígido removido");
    raiz = CriarSala(raiz, "Observatório", "Lente quebrada");

    // ======================= INSERÇÃO DE SUSPEITOS =======================

    inserir_suspeito("Dr. Silva - Médico", "Frasco quebrado");
    inserir_suspeito("Dr. Silva - Médico", "Anotação misteriosa");
    inserir_suspeito("Dr. Silva - Médico", "Toalha molhada");

    inserir_suspeito("Marcos - Chef", "Faca afiada");
    inserir_suspeito("Marcos - Chef", "Guardanapo manchado");
    inserir_suspeito("Marcos - Chef", "Roupa molhada");

    inserir_suspeito("Sofia - Artista", "Pincel sujo de tinta vermelha");
    inserir_suspeito("Sofia - Artista", "Quadro rasgado");
    inserir_suspeito("Sofia - Artista", "Perfume derramado");

    inserir_suspeito("Pedro - Mordomo", "Chave dourada");
    inserir_suspeito("Pedro - Mordomo", "Cofre forçado");
    inserir_suspeito("Pedro - Mordomo", "Luva de couro");

    inserir_suspeito("Otávio - Hacker", "Documento desaparecido");
    inserir_suspeito("Otávio - Hacker", "Disco rígido removido");
    inserir_suspeito("Otávio - Hacker", "Lente quebrada");

    int opcao = 0;  // Variável que guarda a escolha do jogador no menu

    // Loop principal do jogo
    do {
        printf("\n----------------------------------------\n");
        printf("   MANSÃO: %s\n", raiz->valor);
        printf("----------------------------------------\n");

        printf("\n1. Ir para a esquerda\n2. Ir para a direita\n3. Exibir pistas coletadas\n4. Procurar suspeito por pista\n5. Acusar suspeito final\n0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        limparBuffer();  // Limpa caracteres restantes do teclado

        switch (opcao) {
            case 1:
                raiz = PortaEsquerda(raiz, &pistasColetadas);
                printf("Aperte espaço para continuar...\n");
                getchar();
                break;

            case 2:
                raiz = PortaDireita(raiz, &pistasColetadas);
                printf("Aperte espaço para continuar...\n");
                getchar();
                break;

            case 3:
                printf("\nPistas coletadas:\n");
                if (pistasColetadas == NULL)
                    printf("(Nenhuma pista ainda)\n");
                else
                    listarTodasPistas(pistasColetadas);
                printf("\nAperte espaço para continuar...\n");
                getchar();
                break;

            case 4: {
                char pista[50];
                printf("\nDigite o nome da pista: ");
                fgets(pista, sizeof(pista), stdin);
                pista[strcspn(pista, "\n")] = '\0';

                buscar_suspeito_por_pista(pista);

                printf("\nAperte espaço para continuar...\n");
                getchar();
                break;
            }

            case 5: {
                char nome[50];
                printf("\nDigite o nome do suspeito: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                Nodo* suspeito = encontrar_suspeito(nome);

                if (suspeito == NULL) {
                    printf("\n⚠ Suspeito não encontrado!\n");
                } else {
                    int total = contar_pistas_do_suspeito(pistasColetadas, suspeito->pistas);

                    printf("\n🔍 Suspeito: %s\n", suspeito->suspeito);
                    printf("Pistas associadas:\n");

                    PistaNode* p = suspeito->pistas;
                    while (p != NULL) {
                        printf("• %s\n", p->pista);
                        p = p->proximo;
                    }

                    printf("\nPistas encontradas relacionadas: %d\n", total);

                    if (total >= 2) {
                        printf("\n✅ Parabéns! Você descobriu o culpado!\n");
                        opcao = 0; // Encerra o jogo
                    } else {
                        printf("\n❌ Suspeito incorreto! Continue investigando!\n");
                    }
                }

                printf("\nAperte espaço para continuar...\n");
                getchar();
                break;
            }

            case 0:
                printf("\nSaindo do jogo...\n");
                break;

            default:
                printf("\nOpção inválida!\n");
        }

    } while (opcao != 0);

    printf("\nPistas coletadas:\n");
    listarTodasPistas(pistasColetadas);

    printf("\nFim de jogo!\n");
    return 0;
}

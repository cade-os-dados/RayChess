# Movimentação

1. Dar Highlight na peça que está selecionada, além dos possíveis movimentos dela (que já está implementado)

2. Simplificar a API com um Game/GameControler que faça ficar mais fácil reaplicar alguns trechos de código que se repetem...

# Condição de Término do Jogo (V)

Check Mate, não tem para onde o Rei Correr...

Ou simplesmente, eliminaram o rei

## Otimização

Bom para otimizar, acho melhor simplesmente setar uma bool em todas as pieces de, se é para renderizar ou não. Assim, não precisa ficar trocando a peça por NULL. Também precisamos colocar um método para resetar as posições da peça, que são as mesmas de quando a inicializa...
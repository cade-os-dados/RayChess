#include <string>
#include <array>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using Octeto = char[3];
std::array<size_t, 4> dot_idx = {3,6,9}; // index of '.' of ip

bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

void clear_octeto(Octeto& octeto)
{
    octeto[0] = '0';
    octeto[1] = '0';
    octeto[2] = '0';
}

/* Auxiliar com os octetos */
struct IpIndex
{
    size_t value{0};
    inline size_t row()
    {
        return static_cast<size_t>(value / 3);
    }
    inline size_t col()
    {
        return 2 - (value % 3);
    }

    void to_next_octeto()
    {
        if(value < 3) value = 3;
        else if(value < 6) value = 6;
        else if(value < 9) value = 9;
    }

    void to_previous_octeto()
    {
        // Se estiver no segundo octeto (3, 4, 5), volta para o fim do primeiro (2)
        if (value >= 3 && value <= 5) value = 2;
        
        // Se estiver no terceiro octeto (6, 7, 8, 9), volta para o fim do segundo (5)
        else if (value >= 6 && value <= 9) value = 5;
        
        // Se estiver no último octeto (10, 11, 12), volta para o fim do terceiro (9)
        else if (value >= 10) value = 9;
    }

    bool is_last_octeto()
    {
        return value >= 9;
    }

    void next()
    {
        if(value < 12)
            value++;
    }

    void last()
    {
        value--;
    }

    void reset() { value = 0; }
};

class IpString
{
private:
    std::array<Octeto,4> octetos;
    IpIndex idx;
    char last_push = 'x';
public:
    void Debug(){printf("Valor: %d | Coluna: %d\n", idx.value, idx.col());}
    void Clear()
    {
        for(int i = 0; i < 4; i++)
            clear_octeto(octetos[i]);
        idx.reset();
    }
    IpString()
    {
        this -> Clear();
    }

    bool empty(){ return idx.value == 0; }
    bool full() { return idx.value == 12; } // 11 pois índice começa em 0
    std::string str() { return format_canonical( this -> format() ); }
    size_t GetCaretSize() { return idx.value + idx.row(); }

    void push(char c)
    {
        last_push = c;
        if(c == '.')
        {
            // placeholders de onde o . leva
            // talvez futuramente teremos que modificar essa lógica
            if(
                idx.value == 3 
                || idx.value == 6 
                || idx.is_last_octeto()
            ) return;

            idx.to_next_octeto();
            return;
        }
            
        if(is_digit(c))
        {
            // printf("Idx: %d | Char: %c\n", idx.value, c);
            if(idx.col() == 0)
            {
                octetos[idx.row()][0] = octetos[idx.row()][1];
            }
            octetos[idx.row()][1] = octetos[idx.row()][2];
            octetos[idx.row()][2] = c;    
            idx.next();
        }
    }

    void pop()
    {
        if(last_push == '.' && idx.col() == 2)
        {
            idx.to_previous_octeto();
            last_push = 'x';
            return;
        } // se passou deste bloco está nos octetos
   
        if(last_push != 'x')
            idx.last();
            
        /* 
            127 (pull) -> 012 [col = 0]
            012 (pull) -> 001 [col = 1]
            001 (pull) -> 000 [col = 2]
        */
        int i = 2;
        while(i > idx.col())
        {
            octetos[idx.row()][i] = octetos[idx.row()][i-1];
            i--;
        }
        octetos[idx.row()][idx.col()] = '0';
    }

    std::string format() // depois mudo para const char se precisar
    { 
        std::string fmt;
        
        // para cada octeto
        for(auto it = octetos.begin(); it != octetos.end(); it++)
        {
            for(int i = 0; i < 3; i++)
                fmt.push_back((*it)[i]);
            if(it != std::prev(octetos.end()))
                fmt.push_back('.');
        }
        return fmt;
    }

    /*
        Remove os zeros à esquerda dos octetos
        Exemplo:
            in -> "127.000.000.001"
            out -> "127.0.0.1"
    */
    static std::string format_canonical(const std::string& input) {
        std::stringstream ss(input);
        std::string segment;
        std::array<int,4> octets;
        size_t i = 0;
    
        // pega cada octeto e transforma em int -> removendo os leading zeros
        while (std::getline(ss, segment, '.')) {
            if (segment.empty()) throw std::invalid_argument("Segmento vazio");
            int value = std::stoi(segment); // converte para inteiro
            if (value < 0 || value > 255) throw std::out_of_range("Octeto inválido");
            if (i >= 4) throw std::invalid_argument("IP inválido");
            octets[i++] = value;
        }

        // monta a string completa, sem os zeros à esquerda, utilizando os octetos
        return std::to_string(octets[0]) + "." +
               std::to_string(octets[1]) + "." +
               std::to_string(octets[2]) + "." +
               std::to_string(octets[3]);
    }
};
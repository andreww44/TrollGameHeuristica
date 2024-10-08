#include <iostream>
#include <cstdint>
#include "board.hpp"

Board::Board(){

    board[White] = 0x8181818181818181ULL;
    board[Black] = 0xff000000000000ffULL;
    //board[Black] = 0xfffe0800000000ffULL;

    maskBoard[White] = ~board[White];
    maskBoard[Black] = ~board[Black];
    fullMask = maskBoard[White] | maskBoard[Black];
    
    turn = White;
    oneMask = 1ULL;
    zeroMask = 0ULL;

    winingBoard[Black] = 0x00FF000000000000ULL;
    winingBoard[White] = 0x4040404040404040ULL;
    std::bitset<64> bits(winingBoard[Black]);
    std::cout <<  bits << std::endl;
}

Board::Board(const uint16_t white, const uint16_t black, const MARK turn): board{white, black}, turn(turn) {}

U64 Board::getBlackBoard() const { return board[Black]; }
U64 Board::getWhiteBoard() const { return board[White]; }

MARK Board::getActiveTurn() const { return turn; }

Board::~Board() = default;

bool Board::isLegalMove(int position){
    //std::cout << "Wb" << std::bitset<64>(wBoard) << std::endl;
    if(position < 0 || position > BOARD_SIZE)
        return false;
    if (turn == White && ( (board[White] |(board[Black] & maskBoard[Black])) & (oneMask << position) ))          
        return false;
    if (turn == Black && ( (board[Black] |(board[White] & maskBoard[White])) & (oneMask << position) ))
        return false;
    return true;
}

bool Board::isEating(int position){
    // Llama a la función para todas las direcciones
    for (int dir = 0; dir < 8; dir++) {
        eat(position, dir);
    }
    return true;
}

bool Board::eat(int position, int direction){
    
    int y = position / N;
    int x = position % N;
    int dy = DIRECTIONS[direction][0];
    int dx = DIRECTIONS[direction][1];

    // Nueva posición
    int newY = y + dy;
    int newX = x + dx;

    // Comprobar límites
    if(newY < 0 || newY >= N || newX < 0 || newX >= N){
        return false; // Fuera de límites
    }

    int newPosition = newY * N + newX; // Calcular nueva posición en el tablero

    MARK notTurn = (turn == White) ? Black : White;
    
    if(!((board[turn] & maskBoard[turn]) & (oneMask << newPosition))) {
        if(!((board[notTurn] & maskBoard[notTurn]) & (oneMask << newPosition))) {
            return false;
        }
        if(!eat(newPosition, direction)) {
            return false; 
        }
    }
    board[notTurn] &= ~(oneMask << newPosition); 
    board[turn] |= (oneMask << newPosition);     
    return true;
}

bool Board::makeMove(int position){
    if (position < 0 || position >= BOARD_SIZE) {
        return false;
    }
    else if(isLegalMove(position)){
        board[turn] |= (oneMask << position);
        isEating(position);
        turn = (turn == White) ? Black : White;
        return true;
    }
    return false;
}

bool Board::hasWhiteWon(){
    
    U64 map = board[White] & maskBoard[White];  
    U64 visit = zeroMask;  
    
    U64 currentMask = (map & 0xFF00000000000000ULL);  
    
    while (currentMask != 0) {
        visit |= currentMask;  

        U64 rightShift = (currentMask >> 1) & 0x7F7F7F7F7F7F7F7FULL;  
        U64 leftShift = (currentMask << 1) & 0xFEFEFEFEFEFEFEFEULL;   
        U64 downShift = (currentMask >> 8);  
        U64 upShift = (currentMask << 8);    

        // Unimos todos los desplazamientos
        currentMask = (rightShift | leftShift | downShift | upShift) & map;

        if (currentMask & 0x00000000000000FFULL) {
            return true;  // Victoria blanca
        }

        currentMask &= ~visit;
    }

    return false;  // No hay camino completo

}

bool Board::isFull(){
    U64 FB = (board[White] & maskBoard[White]) & (board[Black] & maskBoard[Black]);
    U64 MB = maskBoard[Black] & maskBoard[White];
    return (MB == FB);
}

bool Board::hasBlackWon(){
    
    U64 map = board[Black] & maskBoard[Black];
    
    U64 currentMask = (board[Black] & 0x0101010101010101ULL);  // Primera columna
    U64 visit = 0ULL;

    // Mientras haya piezas por expandir
    while (currentMask) {
        // Añadimos las piezas actuales a la lista de visitados
        visit |= currentMask;

        if (currentMask & 0x8080808080808080ULL) {  
            return true;  // Victoria
        }
        U64 rightShiftMask = (currentMask >> 1) & 0x7F7F7F7F7F7F7F7FULL; 
        U64 leftShiftMask = (currentMask << 1) & 0xFEFEFEFEFEFEFEFEULL;  
        U64 upShiftMask = (currentMask >> N);
        U64 downShiftMask = (currentMask << N);

        currentMask = (rightShiftMask | leftShiftMask | upShiftMask | downShiftMask) & map;
        currentMask &= ~visit;
    }

    return false; 

}

void Board::print(){
    U64 wTable = board[White] & maskBoard[White];
    U64 bTable = board[Black] & maskBoard[Black];
    //U64 table = wTable | bTable; 
    std::cout << "Tablero Troll" << std::endl;
    for (int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++){
            if(wTable&(oneMask << (j+ (i*N)))){
                std::cout<<"W ";
            }
            else if(bTable&(oneMask << (j+ (i*N)))){
                std::cout<<"B ";
            }
            else{
                std::cout<< "* "; 
            }
        }
        std::cout<< "\n"; 
    }
}

MARK Board::getMark(){
    return turn;
}

int Board::evaluateBoard(int depth) {
    
    int score = 0;

    MARK currentTurn = getMark();

    if (hasWhiteWon()){
        if(turn == White){
            if(depth != 0){ return 10000/depth;}
            else{return 10000;}
        }
        else{return -2000*depth;}
    }
    if (hasBlackWon()){
        if(turn == Black){
            if(depth != 0){ return 10000/depth;}
            else{ return 10000; }
        } else{ return -2000*depth;}
    }
    if (isFull())
        return 0;
    if ((maskBoard[currentTurn] & maskBoard[!currentTurn]) == board[currentTurn])
        return 0;

    // Evaluar el número de piezas
    int whitePieces = __builtin_popcountll(board[White]);
    int blackPieces = __builtin_popcountll(board[Black]);
    
    if(turn == Black){
        score -= (whitePieces - blackPieces)*2;
    }
    else{
        score += (whitePieces - blackPieces)*2;
    }
    
    return score;
}

std::vector<int> Board::generateAllLegalMoves() {
    std::vector<int> legalMoves;

    for (int position = 0; position < BOARD_SIZE; ++position) {
        if (isLegalMove(position)) { 
            legalMoves.push_back(position); 
        }
    }
    return legalMoves;
}

//Correcto
bool Board::endGame() {
    return  hasWhiteWon() || hasBlackWon();
}

void Board::undoMove(int pos){
    board[turn] &= ~(oneMask << pos);
    turn = (turn == White) ? Black: White;
}
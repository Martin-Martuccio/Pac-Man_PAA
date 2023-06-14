#include "InterfacciaField.h"

bool IInterfacciaField::isValidPosition(FVector2D position, FVector2D MaxLimit) {
	return isValidPosition(position.X, position.Y, MaxLimit);
}
//Verifica dela vaidità della posizione se rimane dentro il range della mappa
bool IInterfacciaField::isValidPosition(int32 x, int32 y, FVector2D MaxLimit) {
	return x >= 0 && x < MaxLimit.X && y >= 0 && y < MaxLimit.Y;
}

// SWITCH in base al carattere della mappa, posso sapere se la cella è wlkable o no e da chi 
bool IInterfacciaField::isWalkable(char tile, int32 mode) {

	switch (tile) {

	case ' ': 
	case 'o': 
	case '@': 
	case 'T':
	case 'M': 
	case 'N':
	
		return true;

	case 'G': 
	case '+': 
	case 'H':
	
		return mode == 2;

	default:
		
		return mode >= 1 && isGhostHouse(tile);

	}

}

bool IInterfacciaField::isGhostHouse(char tile) {

	switch (tile) {

	case 'B': 
	case 'I': 
	case 'P': 
	case 'C':
	case 'G': 
	case 'H':
	
		return true;

	default:
		
		return false;

	}

}
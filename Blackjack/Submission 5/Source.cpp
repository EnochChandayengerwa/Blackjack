/*
 * File:   blackjack.cpp
 * Author: Steve James <steven.james@wits.ac.za>
 *
 */

#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <sstream>
#include <initializer_list>
#include <algorithm>
#include <random>
namespace {

    class Deck {
    public:

        Deck() {
            mCards.reserve(52);
            for (int rank = 1; rank <= 13; rank++) {
                std::string r = rankToString(rank);
                for (char suit : {'h', 'c', 's', 'd'}) {
                    mCards.push_back(r + suit);
                }
            }
            static auto seed = 42; // fixed seed means repeatable draws
            static std::mt19937 g(seed);
            std::shuffle(mCards.begin(), mCards.end(), g);

        }

        std::string draw() {
            std::string card = mCards.back();
            mCards.pop_back();
            return card;
        }

    private:

        std::string rankToString(int rank) {
            switch (rank) {
            case 1:
                return "A";
            case 11:
                return "J";
            case 12:
                return "Q";
            case 13:
                return "K";
            default:
                return std::to_string(rank);
            }
        }


        std::vector<std::string> mCards;
    };
}

using namespace std;

string outputScore(const vector<string>& hand);
bool isBlackjack(const vector<string>& hand);
bool isBust(const vector<string>& hand);
string getAdvice(const vector<string>& playerHand, string dealerCard);
int getHighScore(const vector<string>& hand);

char getInput(const string& message, initializer_list<char> legalOptions) {
    while (true) {
        cout << message;
        string input;
        cin >> input;
        if (input.empty()) {
            continue;
        }
        char option = toupper(input[0]);

        bool found = false;
        for (char c : legalOptions) {
            if (option == c) {
                found = true;
                break;
            }
        }
        if (found) {
            return option;
        }
        else {
            cout << "Invalid input '" << input << "'" << endl;
        }
    }
}

string handToString(const vector<string>& hand) {
    stringstream ss;
    for (auto card : hand) {
        ss << card << ' ';
    }
    ss << " -> " << outputScore(hand);

    return ss.str();
}

/**
 * Play a single round of Blackjack
 */
void playRound() {
    vector<string> playerHand;
    vector<string> dealerHand;
    Deck deck;
    dealerHand.push_back(deck.draw());
    playerHand.push_back(deck.draw());
    playerHand.push_back(deck.draw());

    cout << "Dealer shows " << handToString(dealerHand) << endl;
    cout << "Player shows " << handToString(playerHand) << endl;
    if (isBlackjack(playerHand)) {
        cout << "Player wins!" << endl;
        return;
    }

    /**
     * Player's turn
     */
    bool playing = true;
    while (playing && !isBust(playerHand)) {

        char action = getInput("(H)it, (S)tand, or (A)dvice? ", { 'H', 'S', 'A' });
        if (action == 'H') {
            playerHand.push_back(deck.draw());
            cout << "Player shows " << handToString(playerHand) << endl;
        }
        else if (action == 'S') {
            playing = false;
        }
        else {
            cout << "Advice: " << getAdvice(playerHand, dealerHand[0]) << endl;
        }
    }

    if (playing) {
        // then player must be bust
        cout << "Dealer wins!" << endl;
        return;
    }

    /**
     * Dealer's turn
     */
    while (!isBust(dealerHand) && getHighScore(dealerHand) < 17) {
        dealerHand.push_back(deck.draw());
        cout << "Dealer shows " << handToString(dealerHand) << endl;
    }

    /**
     * Calculate winner
     */
    if (isBlackjack(dealerHand)) {
        cout << "Dealer wins!" << endl;
    }
    else if (isBust(dealerHand)) {
        cout << "Player wins!" << endl;
    }
    else {
        int playerScore = getHighScore(playerHand);
        int dealerScore = getHighScore(dealerHand);
        if (playerScore > dealerScore) {
            cout << "Player wins!" << endl;
        }
        else if (playerScore < dealerScore) {
            cout << "Dealer wins!" << endl;
        }
        else {
            cout << "Push!" << endl;
        }
    }
}

int main() {
    bool running = true;
    while (running) {
        char option = getInput("(N)ew round or (Q)uit? ", { 'N', 'Q' });
        if (option == 'N') {
            playRound();
            cout << "******************************************" << endl;
        }
        else if (option == 'Q') {
            running = false;
        }
    }

    return 0;
}


//Implement the functions below this line!

/**
 * Function that takes in a hand and returns the value(s)
 * @param hand the hand
 * @return the value or values (if there is an ace) of the hand
 */
string outputScore(const vector<string>& hand) {
    string c1, v1;
    int count = 0, total1 = 0, total2 = 0;

    for (int i = 0; i < hand.size(); i++) {
        c1 = hand[i];
        v1 = c1.substr(0, (c1.size() - 1));

        if (v1 == "J" || v1 == "Q" || v1 == "K") {
            v1 = "10";
        }

        if (v1 == "A") {
            if (total1 < 11) {
                total1 += 11;
            }
            else {
                total1 += 1;
            }
            total2 += 1;
        }
        else {
            total1 += atoi(v1.c_str());
            total2 += atoi(v1.c_str());
        }

        count++;
    }

    if (count == 2) {
        if (total1 == 21 || total2 == 21) {
            return "Blackjack!";
        }
        else if (total1 > 21 && total2 > 21) {
            return "Bust!";
        }
        else if (total1 > 21 && total2 < 21) {
            return to_string(total2);
        }
        else if (total1 == total2) {
            return to_string(total1);
        }
        else {
            return to_string(total2) + " or " + to_string(total1);
        }
    }
    else {
        if (total1 > 21 && total2 > 21) {
            return "Bust!";
        }
        else if (total1 > 21 && total2 < 21) {
            return to_string(total2);
        }
        else if (total1 == total2) {
            return to_string(total1);
        }
        else {
            return to_string(total2) + " or " + to_string(total1);
        }
    }
}

/**
 * Whether the hand represents a Blackjack hand
 * @param hand the hand
 * @return true if the hand is Blackjack. False otherwise
 */
bool isBlackjack(const vector<string>& hand) {
    if (outputScore(hand) == "BlackJack!") {
        return true;
    }
    else {
        return false;
    }
}

/**
 * Whether the hand is bust
 * @param hand the hand
 * @return whether the hand's value exceeds 21
 */
bool isBust(const vector<string>& hand) {
    if (outputScore(hand) == "Bust!") {
        return true;
    }
    else {
        return false;
    }
}

/**
 * Computes a suggestion as to the action to take given the cards
 * @param playerHand the player's hand
 * @param dealerCard the dealer's single card
 * @return either 'Hit' or 'Stand'
 */
string usableAce(int pc, string dc) {
    if (pc<18) { return "Hit"; }
    if (pc == 18) {
        if (dc == "9" || dc == "10" || dc == "A") { return "Hit"; }
        else { return "Stand"; }
    }
    if (pc > 18) { return "Stand"; }
}
string noAce(int pc, string dc) {

    if (pc <= 11) { return "Hit"; }
    if (pc == 12) {
        if (dc != "4" && dc != "5" && dc != "6") {
            return "Hit";
        }
        else {
            return "Stand";
        }
    }
    if (pc > 12 && pc < 17) {
        if (dc == "2" || dc == "3" || dc == "4" || dc == "5" || dc == "6") {
            return "Stand";
        }
        else {
            return "Hit";
        }
    }
    if (pc > 17) { return "Stand"; }
}

string getAdvice(const vector<string>& playerHand, string dealerCard) {
    int pc = 0;
    string dc = dealerCard.substr(0, dealerCard.length() - 1);

    if (outputScore(playerHand).find("or")!= std::string::npos) {
        pc = atoi(outputScore(playerHand).substr(outputScore(playerHand).find("r")+1, outputScore(playerHand).length()).c_str());
        //cout << "IN THIS CASE ITS: "<<pc << endl;
        return usableAce(pc, dc);
    }
    else {
        pc = atoi(outputScore(playerHand).substr(0, outputScore(playerHand).find(" ")).c_str());
        return noAce(pc, dc);

    }

}

/**
 * Returns the maximum (non-bust) value of the hand. If the hand contains an ace,
 * this means the higher of the two values is picked, provided it does not exceed
 * 21.
 * @param hand the hand
 * @return the best possible score for the hand
 */
int getHighScore(const vector<string>& hand) {
    if (outputScore(hand).find("or") != std::string::npos) {
        return atoi(outputScore(hand).substr(outputScore(hand).find("r")+1, outputScore(hand).size()).c_str());
    }
    else
    {
        return atoi(outputScore(hand).c_str());
    }
}
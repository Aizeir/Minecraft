#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


class Viennoiserie {
    int prix = 1;
    string nom;

public:
    Viennoiserie(string nom_) {
        nom = nom_;
    }

    void interagir() {
        string nom_up = nom;
        transform(nom_up.begin(), nom_up.end(), nom_up.begin(), ::toupper);
        cout << nom_up << " [" << prix << "€] à été INTERAGI !!!!🎂🥐🥪🍞🥫🥯🍠🥨🍞🍳" << endl << "🥪🥙🍘🥡🍣🍛🥘🧆🍧🎂🧁🧁🧁🧁" << endl << "🍌🍋🍑🍏🍈🍯🥗🥗🧀🧀🥞🍳🍕🥚🏓🏓🏸🧩🧸🧸🧸🧸🧸🧸🧸" << endl << "C'était le INTERAGI. ■" << endl;
    }
};


int money = 10;

void check_money(int money) {
    if (money < 0) {
        cout << "tu as moins que 0 argent (" << money << ") mais pas grave !" << endl;
        money = 0;
    }
}

int main() {
    cout << "Bienvenue !!! Tu as " << money << " argent !" << endl;
    cout << "Tu achete combien de pain au raisins à 1€ ? : ";
    
    string x;
    cin >> x;
    money -= atoi(x.c_str());
    cout << "Tu as " << x << " pains aux raisins mais plus que " << money << " euros." << endl;
    check_money(money);

    string friandises[] = {"pain au chocolat (pas chocolatine)","croissant","chausson aux pommes","flan","muffin","sandwich au saumon","CHOUQUETTES","gateau","tarte","cookie"};
    int calories = 0;
    string ans;

    vector<string> paye;
    if (x != "0"){
    for (int i=0; i < atoi(x.c_str()); i++) {
        paye.push_back("pain au RAISIN");
    }
    }

    int money_left = money;

    for (int i=0; i < money_left; i++) {
        do {
            cout << "Acheter le " << friandises[i] << " ? : "; cin >> ans;
            if (ans == "oui") {
                money -= 1;
                check_money(money);
                calories++;
                cout << "Tu as converti 1 euro en 200 calories !" << endl;
                paye.push_back(friandises[i]);
                break;
            }
            else if (ans == "non") {
                cout << "Sage décision." << endl;
                break;
            }
            else {
                cout << "Dis \"oui\" ou \"non\" !" << endl;
                continue;
            }
            break;
        }
        while (1);
    }
    cout << endl;
    cout << "Bilan: " << money << " euros, " << calories << " grammes de gras a bruler le lendemain !" << endl;
    cout << "TRUCS ENGLOUTIS: ";
    for (int i=0; i<paye.size(); i++) {
        cout << paye[i];
        if (i < paye.size()-1) cout << ", ";
        else cout << " !!!!!" << endl<<endl;
    }

    Viennoiserie truc = Viennoiserie(paye[rand() % paye.size()]);
    truc.interagir();
    
    cout << "Voilà ■";
    return 0;
}
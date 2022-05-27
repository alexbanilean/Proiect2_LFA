#include <bits/stdc++.h>

using namespace std;

struct triple{
    int x;
    char l;
    int y;

    friend ostream &operator<<(ostream &os, const triple &triple) {
        os << "{" << triple.x << ", " << triple.l << ", " << triple.y << "}";
        return os;
    }
};

template <typename T>
ostream& operator<<(ostream& os, const vector<T> v) {
    for(const auto x: v)
        os << x << " ";

    os << "\n";

    return os;
}

template <typename T>
ostream& operator<<(ostream& os, const set<T> v) {
    for(const auto x: v)
        os << x << " ";

    os << "\n";

    return os;
}

class automat {

protected:
    int nr_stari, nr_tranzitii, stare_initiala, nr_stari_finale;
    vector<int> stari, stari_finale;
    vector<triple> tranzitii;
    set<char> alfabet;
    map<pair<int, char>, set<int>> tabel;

public:

    automat(int nrStari, int nrTranzitii, int stareInitiala, int nrStariFinale, const vector<int> &stari,
            const vector<int> &stariFinale, const vector<triple> &tranzitii) : nr_stari(nrStari),
                                                                               nr_tranzitii(nrTranzitii),
                                                                               stare_initiala(stareInitiala),
                                                                               nr_stari_finale(nrStariFinale),
                                                                               stari(stari), stari_finale(stariFinale),
                                                                               tranzitii(tranzitii) {
        for(auto x0: tranzitii) {
            this->alfabet.insert(x0.l);
        }
    }

    set<int> get_closure(int stare, char litera){
        set<int> temp = {};
        for(auto x: tranzitii)
            if(x.x == stare && x.l == litera) {
                temp.insert(x.y);
            }
        return temp;
    }

    void create_table(){
        for(auto st: stari)
            for (auto lt: alfabet) {
                tabel.insert({{st, lt}, get_closure(st, lt)});
            }
    }

    void display_table(){
        for(const auto& x : tabel){
            cout << x.first.first << " " << x.first.second << ":\n";
            cout << x.second;
        }
    }

    friend istream &operator>>(istream &is, automat &automat) {
        cout << "nr_stari: ";
        is >> automat.nr_stari;
        cout << "\nnr_tranzitii: ";
        is >> automat.nr_tranzitii;
        cout << "\nstare_initiala: ";
        is >> automat.stare_initiala;
        cout << "\nnr_stari_finale: ";
        is >> automat.nr_stari_finale;

        cout << "\nstari: ";
        for (int i = 0; i < automat.nr_stari; ++i) {
            int aux;
            is >> aux;
            automat.stari.push_back(aux);
        }

        cout << "\ntranzitii: ";
        for (int i = 0; i < automat.nr_tranzitii; ++i) {
            triple x0{};
            is >> x0.x >> x0.y >> x0.l;
            automat.tranzitii.push_back(x0);
            automat.alfabet.insert(x0.l);
        }

        cout << "\nstari_finale: ";
        for (int i = 0; i < automat.nr_stari_finale; ++i) {
            int aux;
            is >> aux;
            automat.stari_finale.push_back(aux);
        }

        return is;
    };

    friend ostream &operator<<(ostream &os, const automat &automat) {
        os << "nr_stari: " << automat.nr_stari << "\nnr_tranzitii: " << automat.nr_tranzitii << "\nstare_initiala: "
           << automat.stare_initiala << "\nnr_stari_finale: " << automat.nr_stari_finale << "\nstari: " << automat.stari
           << "\ntranzitii: " << automat.tranzitii << "\nstari_finale: " << automat.stari_finale << "\nalfabet: "
           << automat.alfabet;
        return os;
    };
};

class DFA: public automat{

public:

    using automat::automat;

    void minimizeDFA(){

        this->create_table();
//        this->display_table();

        vector<int> initial_state(nr_stari + 1, 1), next_state(nr_stari + 1, -1), junk(nr_stari + 1, -1);
        vector<set<int>> states(2);

        for(int i = 0; i < stari_finale.size(); ++i){
            initial_state[stari_finale[i]] = 0;
            states[0].insert(stari_finale[i]);
        }

        for(auto x: stari)
            if(states[0].find(x) == states[0].end())
                states[1].insert(x);

        int nr_partitii = 2;

        while(initial_state != next_state){

            next_state = initial_state;

            for(int i = 0; i < nr_partitii; ++i){
                if(states[i].size() > 1){
                    set<int> new_partition = {};
                    auto key_state = *states[i].begin();

                    for(auto x : states[i]){
                        if(x != key_state){
                            /// compare states here
                            bool equi_flag = true;

                            for (auto litera: alfabet)
                                if (initial_state[*tabel.find({key_state, litera})->second.begin()] !=
                                    initial_state[*tabel.find({x, litera})->second.begin()]){
                                    equi_flag = false; break;
                                }

                            if(!equi_flag) {
                                 // distinguishable states
                                 new_partition.insert(x);
                            }
                        }
                    }

                    if(!new_partition.empty()){
                        states.push_back(new_partition);

                        for(auto x : new_partition) {
                            next_state[x] = nr_partitii;
                            states[i].erase(x);
                        }

                        ++nr_partitii;
                    }
                }
            }

        }

        cout << "DFA minimizat cu " << states.size() << " stari: \n\n";
        for(auto x: states) {
            // Stare
            cout << "Stare: ";
            for (auto y: x)
                cout << y << ' ';
            cout << '\n';

            //Litera + inchidere
            for(auto litera: alfabet){
                cout << "Litera: " << litera << " Inchidere: ";
                set<int> closure = {};

                for(auto y : x) {
                    auto aux = tabel.find({y, litera})->second;
                    closure.insert(aux.begin(), aux.end());
                }

                for(auto elem : closure)
                    cout << elem << ' ';
                cout << '\n';
            }
            cout << '\n';
        }

    }
};

class NFA: public automat{

public:

    using automat::automat;

    void convertToDFA(){

        this->create_table();
//        this->display_table();

        set<tuple<set<int>, char, set<int>>> lista_finala_conversie;
        queue<set<int>> Q;
        Q.push({stare_initiala});

        while(!Q.empty()){
            auto stare_curenta = Q.front();
            Q.pop();

            for(auto litera: alfabet){

                /// inchidere

                set<int> inchidere = {};

                for(auto k:stare_curenta) {
                    auto inchidere_temp = tabel.find({k,litera});
                    if(inchidere_temp != tabel.end() && !inchidere_temp->second.empty())
                        inchidere.insert(inchidere_temp->second.begin(), inchidere_temp->second.end());
                }

                ///

                auto elem = make_tuple(stare_curenta, litera, inchidere);

                if(!lista_finala_conversie.empty()){
                    if(lista_finala_conversie.find(elem) == lista_finala_conversie.end()) {
                        lista_finala_conversie.insert(elem);
                        if(!inchidere.empty())
                            Q.push(inchidere);
                    }
                }
                else{
                        lista_finala_conversie.insert(elem);
                        if(!inchidere.empty())
                            Q.push(inchidere);
                }
            }
        }

        set<set<int>> stari_afisate;

        cout << "DFA echivalent: \n\n";
        for(auto x: lista_finala_conversie){
            if(stari_afisate.find(get<0>(x)) == stari_afisate.end()) {
                stari_afisate.insert(get<0>(x));
                cout << "Stare: " << get<0>(x);

                for (auto y: lista_finala_conversie)
                    if (get<0>(y) == get<0>(x))
                        cout << "Litera: " << get<1>(y) << " Inchidere: " << get<2>(y);
                cout << '\n';
            }
        }
    }
};

int main(){

//    NFA automat_initial(6, 7, 1, 2,
//                        {1, 2, 3, 4, 5, 6},
//                        {5, 6},
//                        {
//                            {1, 'a', 2},
//                            {1, 'b', 4},
//                            {4, 'b', 6},
//                            {1, 'b', 3},
//                            {2, 'b', 5},
//                            {5, 'b', 5},
//                            {5, 'a', 5}
//                        }
//                        );

    NFA automat_initial(3, 4, 1, 1,
                        {1, 2, 3},
                        {3},
                        {
                                {1, 'a', 1},
                                {1, 'b', 1},
                                {1, 'a', 2},
                                {2, 'b', 3}
                        }
    );

    automat_initial.convertToDFA();

//    DFA automat_input(6, 12, 1, 1,
//                      {1, 2, 3, 4, 5, 6},
//                      {5},
//                      {
//                              {1, 'b', 2},
//                              {1, 'a', 4},
//                              {2, 'a', 3},
//                              {2, 'b', 6},
//                              {3, 'a', 3},
//                              {3, 'b', 6},
//                              {4, 'a', 1},
//                              {4, 'b', 5},
//                              {5, 'a', 3},
//                              {5, 'b', 6},
//                              {6, 'a', 6},
//                              {6, 'b', 6}
//                      }
//                      );

    DFA automat_input(6, 12, 1, 3,
                      {1, 2, 3, 4, 5, 6},
                      {2, 3, 5},
                      {
                              {1, 'b', 2},
                              {1, 'a', 4},
                              {2, 'a', 3},
                              {2, 'b', 6},
                              {3, 'a', 3},
                              {3, 'b', 6},
                              {4, 'a', 1},
                              {4, 'b', 5},
                              {5, 'a', 3},
                              {5, 'b', 6},
                              {6, 'a', 6},
                              {6, 'b', 6}
                      }
    );

    automat_input.minimizeDFA();

    return 0;
}

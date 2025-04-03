#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;

class Hero {
public:
    int level;
    double hp, defence, agility, attack1, attack2, elo;
    int win, draw, lose;

    Hero(int cấp, double máu, double thủ, double mẫn, double công1, double công2, double elo = 2000, int thắng = 0, int hòa = 0, int thua = 0)
        : level(cấp), hp(máu), defence(thủ), agility(mẫn), attack1(công1), attack2(công2), elo(elo), win(thắng), draw(hòa), lose(thua) {}

    string toString() const {
        int total = win + draw + lose;
        ostringstream oss;
        if (total > 0) {
            oss << "(cấp " << level << ") HP: " << hp << ", DEF: " << fixed << setprecision(1) << defence
                << ", AGI: " << agility << ", ATK: " << attack1 << "-" << attack2
                << ", ELO: " << fixed << setprecision(2) << elo
                << ", Rate: " << total << " [" << fixed << setprecision(1)
                << (win * 100.0 / total) << "%/" << (draw * 100.0 / total) << "%/" << (lose * 100.0 / total) << "%]";
        } else {
            oss << "(cấp " << level << ") HP: " << hp << ", DEF: " << fixed << setprecision(1) << defence
                << ", AGI: " << agility << ", ATK: " << attack1 << "-" << attack2
                << ", ELO: " << fixed << setprecision(2) << elo
                << ", Rate: " << total;
        }
        return oss.str();
    }
};

double gain(double A, double B, double score, double K = 1) {
    double Qa = pow(10, A / 400), Qb = pow(10, B / 400);
    double E = K * (score - Qa / (Qa + Qb));
    return A + E;
}

double vs(const Hero& A, const Hero& B, int turn = 100) {
    double hp1 = A.hp, hp2 = B.hp;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> disA(A.attack1, A.attack2);
    uniform_real_distribution<> disB(B.attack1, B.attack2);

    for (int i = 0; i < turn; ++i) {
        hp2 -= max(disA(gen) - B.defence, 0.0) * A.agility / B.agility;
        if (hp2 < 0) return 1;
        hp1 -= max(disB(gen) - A.defence, 0.0) * B.agility / A.agility;
        if (hp1 < 0) return 0;
    }
    return 0.5;
}

// Lưu ý:
// - turn = 100: ưu thế nhất là nâng agi + atk
// - turn = 200: ưu thế nhất là nâng def + atk
// - turn = 300: ưu thế nhất là nâng def + agi + atk
// - turn = 400: ưu thế nhất là nâng def
double vs2(Hero& A, Hero& B, int turn = 100) {
    double hpA = A.hp, hpB = B.hp;
    double a = 0, b = 50;
    int turnA = 0, turnB = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> disA(A.attack1, A.attack2);
    uniform_real_distribution<> disB(B.attack1, B.attack2);

    while (turnA < turn || turnB < turn) {
        if (a == b) {
            hpB -= max(disA(gen) - B.defence, 0.0);
            a += B.agility;
            turnA++;
            hpA -= max(disB(gen) - A.defence, 0.0);
            b += A.agility;
            turnB++;
        } else if (a < b) {
            hpB -= max(disA(gen) - B.defence, 0.0);
            a += B.agility;
            turnA++;
        } else {
            hpA -= max(disB(gen) - A.defence, 0.0);
            b += A.agility;
            turnB++;
        }
        if (hpA < 0 && hpB < 0) return 0.5;
        if (hpB < 0) return 1;
        if (hpA < 0) return 0;
    }
    return 0.5;
}

void attack(Hero& A, Hero& B, int N) {
    for (int i = 0; i < N; ++i) {
        double score = vs2(A, B);
        double newEloA = gain(A.elo, B.elo, score);
        double newEloB = gain(B.elo, A.elo, 1 - score);
        A.elo = newEloA;
        B.elo = newEloB;

        if (score == 1) {
            A.win++;
            B.lose++;
        } else if (score == 0) {
            A.lose++;
            B.win++;
        } else {
            A.draw++;
            B.draw++;
        }
    }
}

vector<Hero> heroes;

void add_hero(int p1, int p2, int p3, int p4) {
    int level = p1 + p2 + p3 + p4;
    double hp = p1 * 321;
    double de = p2 * 3.1;
    double ag = p3 * 4.1;
    double at = p4 * 2.4;
    heroes.emplace_back(level, hp + 5000 + level * 100, de, ag + 100, at, at + 100 + 2 * level);
}

void add_hero_level(int sp) {
    for (int p1 = 0; p1 <= sp; ++p1) {
        for (int p2 = 0; p2 <= sp - p1; ++p2) {
            for (int p3 = 0; p3 <= sp - p1 - p2; ++p3) {
                int p4 = sp - p1 - p2 - p3;
                add_hero(p1, p2, p3, p4);
            }
        }
    }
}

void train(int count = 1000000) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, heroes.size() - 1);

    int n = 0;
    while (n < count) {
        Hero& A = heroes[dis(gen)];
        Hero& B = heroes[dis(gen)];
        if (&A == &B || abs(A.elo - B.elo) >= 400) continue;
        attack(A, B, 1);
        n++;
    }
}

void train2(int limit = 400) {
    for (size_t a = 0; a < heroes.size(); ++a) {
        for (size_t b = a + 1; b < heroes.size(); ++b) {
            if (limit > abs(heroes[a].elo - heroes[b].elo)) {
                attack(heroes[a], heroes[b], 1);
            }
        }
    }
}

void train3(int round = 10) {
    vector<vector<int>> r(round, vector<int>(heroes.size(), -1));

    cout << "Thực hiện " << round << " vòng đấu ==========================" << endl;
    // đấu round vòng
    for (int i = 0; i < round; ++i) {
        // tạo danh sách theo elo giảm dần
        vector<int> index(heroes.size());
        iota(index.begin(), index.end(), 0);
        sort(index.begin(), index.end(), [](int a, int b) {
            return heroes[a].elo > heroes[b].elo;
        });
        cout << "Vòng " << i+1 << ": " << heroes[index[0]].toString() << endl;

        // đấu các hero trong danh sách
        for (int k = 0; k < heroes.size(); ++k)
            if (r[i][index[k]] == -1) {
                int p = k;
                while (true) {
                    p++;
                    bool found = false;
                    for (int t = 0; t < i; ++t)
                        if (r[t][index[k]] == p) {
                            found = true;
                            break;
                        }
                    if (!found) break;
                }
                if (p >= heroes.size()) break;
                r[i][index[k]] = p;
                r[i][index[p]] = k;
                attack(heroes[index[k]], heroes[index[p]], 10);
            }
    }
}

int main() {
    /*
    vector<Hero> h;
    for (int i = 1; i <= 40; ++i) {
        heroes.clear();
        add_hero_level(i);
        random_device rd;
        mt19937 gen(rd());
        shuffle(heroes.begin(), heroes.end(), gen);
        h.insert(h.end(), heroes.begin(), heroes.begin() + min(10, (int)heroes.size()));
    }
    heroes = h;
    random_device rd;
    mt19937 gen(rd());
    shuffle(heroes.begin(), heroes.end(), gen);
    */

    add_hero(0, 0, 0, 0);
    for (int lv = 1; lv <= 45; ++lv)
        add_hero_level(lv);
    /*
    for (int lv = 1; lv <= 120; ++lv) {
        add_hero(lv, 0, 0, 0);
        add_hero(0, lv, 0, 0);
        add_hero(0, 0, lv, 0);
        add_hero(0, 0, 0, lv);
    }
    for (int lv = 1; lv <= 60; ++lv) {
        add_hero(lv, lv, 0, 0);
        add_hero(lv, 0, lv, 0);
        add_hero(lv, 0, 0, lv);
        add_hero(0, lv, lv, 0);
        add_hero(0, lv, 0, lv);
        add_hero(0, 0, lv, lv);
    }
    for (int lv = 1; lv <= 40; ++lv) {
        add_hero(lv, lv, lv, 0);
        add_hero(lv, lv, 0, lv);
        add_hero(lv, 0, lv, lv);
        add_hero(0, lv, lv, lv);
    }        
    for (int lv = 1; lv <= 30; ++lv) {
        add_hero(lv, lv, lv, lv);
    }
    for (int lv = 1; lv <= 20; ++lv) {
        add_hero(lv+lv+lv, lv, lv, lv);
        add_hero(lv+lv, lv+lv, lv, lv);
        add_hero(lv+lv, lv, lv+lv, lv);
        add_hero(lv+lv, lv, lv, lv+lv);
        add_hero(lv, lv+lv+lv, lv, lv);
        add_hero(lv, lv+lv, lv+lv, lv);
        add_hero(lv, lv+lv, lv, lv+lv);
        add_hero(lv, lv, lv+lv+lv, lv);
        add_hero(lv, lv, lv+lv, lv+lv);
        add_hero(lv, lv, lv, lv+lv+lv);
    }
    */

    random_device rd;
    mt19937 gen(rd());
    shuffle(heroes.begin(), heroes.end(), gen);

    cout << "Tất cả " << heroes.size() << " heroes ==========================" << endl;
    for (int i = 1; i <= 10; ++i) {
        train3(100);
        auto maxHero = *max_element(heroes.begin(), heroes.end(), [](const Hero& a, const Hero& b) {
            return a.elo < b.elo;
        });
        cout << "Vòng " << i << ": " << maxHero.toString() << endl;
    }

    cout << "Xếp theo ELO ===============" << endl;
    sort(heroes.begin(), heroes.end(), [](const Hero& a, const Hero& b) {
        return a.elo < b.elo;
    });
    for (const auto& h : heroes) {
        cout << h.toString() << endl;
    }
    
    cout << "Xếp theo Tỉ lệ thắng ===============" << endl;
    sort(heroes.begin(), heroes.end(), [](const Hero& a, const Hero& b) {
        double rateA = a.win / (double)(a.win + a.draw + a.lose);
        double rateB = b.win / (double)(b.win + b.draw + b.lose);
        return rateA < rateB;
    });
    for (const auto& h : heroes) {
        cout << h.toString() << endl;
    }

    cout << "Xếp theo Tỉ lệ thua ===============" << endl;
    sort(heroes.begin(), heroes.end(), [](const Hero& a, const Hero& b) {
        double rateA = a.lose / (double)(a.win + a.draw + a.lose);
        double rateB = b.lose / (double)(b.win + b.draw + b.lose);
        return rateA < rateB;
    });
    for (const auto& h : heroes) {
        cout << h.toString() << endl;
    }


    return 0;
}
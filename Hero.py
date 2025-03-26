import random

# lớp Hero: chứa thông tin về Hero
class Hero:
    def __init__(self, cấp, máu, thủ, mẫn, công1, công2, elo = 2000, thắng = 0, hòa = 0, thua = 0):
        self.level = cấp        # mỗi cấp được thêm 1 điểm sp phân bổ vào các thuộc tính
        self.hp = máu           # lượng máu, bị trừ đi mỗi khi bị đánh
        self.defence = thủ      # trừ lực đánh của đối phương
        self.agility = mẫn      # khả năng nhân lực đánh (so chỉ số với đối phương)
        self.attack1 = công1    # lực đánh tối thiểu
        self.attack2 = công2    # lực đánh tối đa
        self.elo = elo
        self.win = thắng
        self.draw = hòa
        self.lose = thua

    def __str__(self):
        total = self.win + self.draw + self.lose
        if total > 0:
            return f'(cấp {self.level}) HP: {self.hp}, DEF: {self.defence:0.1f}, AGI: {self.agility:0.1f}, ATK: {self.attack1:0.1f}-{self.attack2:0.1f}, ELO: {self.elo:0.2f}, Rate: {total} [{self.win * 100 / total:0.1f}%/{self.draw * 100 / total:0.1f}%/{self.lose * 100 / total:0.1f}%]'
        else:
            return f'(cấp {self.level}) HP: {self.hp}, DEF: {self.defence:0.1f}, AGI: {self.agility:0.1f}, ATK: {self.attack1:0.1f}-{self.attack2:0.1f}, ELO: {self.elo:0.2f}, Rate: {total}'

# trả về hệ số elo sau khi trận đấu A vs B có kết quả score (thắng/hòa/thua ~ 1/0.5/0)
def gain(A, B, score, K = 10):
    Qa, Qb = 10 ** (A / 400), 10 ** (B / 400)
    E = K * (score - Qa / (Qa + Qb))
    return A + E, B - E

# cho A đấu với B, trả về kết quả (thắng/hòa/thua ~ 1/0.5/0)
# tối đa 100 lượt đánh (turn)
def vs(A, B, turn = 100):
    hp1, hp2 = A.hp, B.hp
    for i in range(turn):
        hp2 -= max(random.uniform(A.attack1, A.attack2) - B.defence, 0) * A.agility / B.agility
        if hp2 < 0: return 1
        hp1 -= max(random.uniform(B.attack1, B.attack2) - A.defence, 0) * B.agility / A.agility
        if hp1 < 0: return 0
    return 0.5

# tốc độ đánh là 1/a và 1/b, tức là b/ab và a/ab
# A đánh trước, bắt đầu vào các thời điểm 0, b, 2b, 3b,...
# B đánh sau, bắt đầu vào các thời điểm 1, 1+a, 1+2a, 1+3a,...
# kết thúc sau khi cả 2 đủ số turn
# chú ý xử lý trường hợp cả 2 cùng ra chiêu
def vs2(A, B, turn = 100):
    hpA, hpB = A.hp, B.hp
    a, b = 0, 50
    turnA, turnB = 0, 0
    while turnA < turn or turnB < turn:
        if a == b:
            hpB -= max(random.uniform(A.attack1, A.attack2) - B.defence, 0)
            a += B.agility
            turnA += 1
            hpA -= max(random.uniform(B.attack1, B.attack2) - A.defence, 0)
            b += A.agility
            turnB += 1
        elif a < b:
            hpB -= max(random.uniform(A.attack1, A.attack2) - B.defence, 0)
            a += B.agility
            turnA += 1
        else:
            hpA -= max(random.uniform(B.attack1, B.attack2) - A.defence, 0)
            b += A.agility
            turnB += 1
        if hpA < 0 and hpB < 0: return 0.5
        if hpB < 0: return 1
        if hpA < 0: return 0
    else: return 0.5

# cho A và B đấu N trận, cập nhật các hệ số
def attack(A, B, N):
    for i in range(N):
        score = vs2(A, B)
        A.elo, B.elo = gain(A.elo, B.elo, score)
        if score == 1:
            A.win += 1
            B.lose += 1
        elif score == 0:
            A.lose += 1
            B.win += 1
        else:
            A.draw += 1
            B.draw += 1

# nạp dữ liệu từ file
def loadfile():
    pass

# ghi dữ liệu ra file
def savefile():
    pass

# thêm hero mới vào bộ dữ liệu
def add_hero(p1, p2, p3, p4):
    level = p1 + p2 + p3 + p4
    hp = p1 * 321
    de = p2 * 3.1
    ag = p3 * 4.1
    at = p4 * 2.4
    # hp gốc 5000, mỗi level thêm 100
    # at tối đa mỗi level thêm 200 (trung bình tăng 100)
    heroes.append(Hero(level, hp + 5000 + level * 100, de, ag + 100, at, at + 100 + 2 * level))

# thêm mọi phương án hero có tổng điểm thuộc tính là sp (tương đương với level = sp)
def add_hero_level(sp):
    for p1 in range(sp + 1):
        for p2 in range(sp - p1 + 1):
            for p3 in range(sp - p1 - p2 + 1):
                p4 = sp - p1 - p2 - p3
                add_hero(p1, p2, p3, p4)

# chọn ngẫu nhiên count cặp hero cho đấu nhau
def train(count = 1_000_000):
    n = 0
    while n < count:
        A = random.choice(heroes)
        B = random.choice(heroes)
        if (A == B) or (400 <= abs(A.elo - B.elo)): continue
        attack(A, B, 1)
        n = n + 1

# lần lượt cho mọi hero đấu với nhau
def train2(limit = 400):
    for a in range(len(heroes)):
        for b in range(a + 1, len(heroes)):
            if limit > abs(heroes[a].elo - heroes[b].elo):
                attack(heroes[a], heroes[b], 1)

h = []
heroes = []
for i in range(300,301):
    heroes = []
    add_hero_level(i)
    h.extend(random.sample(heroes, min(500, len(heroes))))
    #h.extend(heroes)
heroes = h
#add_hero(0, 0, 0, 0)
heroes = random.sample(heroes, min(200, len(heroes)))
random.shuffle(heroes)
print(f"Tất cả {len(heroes)} heroes ==========================")
for i in range(100):
    train2()
    print(f"Vòng {i}: {max(heroes, key = lambda h: h.elo)}")
print("Xếp theo ELO ===============")
for h in sorted(heroes, key = lambda h: h.elo): print(h)
print("Xếp theo Ti lệ thắng ===============")
for h in sorted(heroes, key = lambda h: (h.win / (h.win + h.draw + h.lose), h.draw / (h.win + h.draw + h.lose))): print(h)
print("Xếp theo Tỉ lệ thua ===============")
for h in sorted(heroes, key = lambda h: (h.lose / (h.win + h.draw + h.lose), h.draw / (h.win + h.draw + h.lose))): print(h)

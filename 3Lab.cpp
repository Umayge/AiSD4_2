#include <iostream>
#include <string>
#include <clocale>
#include <list>
#include <time.h>
#include <vector>
#include <stack>
#include <iterator>
#include <algorithm>
#include <exception>
#include <locale>

using namespace std;

struct Node {	
    int key, h;	
    Node* L[2];	
    Node(int k) : key(k), h(1) { L[0] = L[1] = nullptr; }
    ~Node() { delete L[1]; delete L[0]; }
    void Display(int, int, int); 
    Node(const Node&) = delete;
    Node& operator = (const Node&) = delete;
    
    friend int height(Node*& p)
    {
        return p ? p->h : 0;
    }
    int balancefactor()
    {
        return height(L[1]) - height(L[0]);
    }

    int fixheight()
    {
        auto hl = height(L[0]);
        auto hr = height(L[1]);
        h = (hl > hr ? hl : hr) + 1;
        return h;
    }


    //	friend class Tree;
};

using MyStack = stack<pair<Node*, int>>;

struct myiter : public iterator<forward_iterator_tag, int>
{
    Node* Ptr;
    MyStack St;
    myiter(Node* p = nullptr) : Ptr(p) {} 
    myiter(Node* p, const MyStack&& St) : Ptr(p), St(move(St)) {}
    bool operator == (const myiter& Other) const { return Ptr == Other.Ptr; }
    bool operator != (const myiter& Other) const { return !(*this == Other); }
    myiter& operator++();
    myiter operator++(int) { myiter temp(*this); ++* this; return temp; }
    pointer operator->() { return &Ptr->key; }
    reference operator*() { return Ptr->key; }
};

template <typename Container, typename Iter = myiter>
class outiter : public iterator<output_iterator_tag, typename Container::value_type>
{
protected:
    Container& container;    
    Iter iter;	
public:
    explicit outiter(Container& c, Iter it) : container(c), iter(it) { }
    const outiter<Container>&
        operator = (const typename Container::value_type& value) {
        iter = container.insert(value, iter).first;
        return *this;
    }
    const outiter<Container>& 
        operator = (const outiter<Container>&) { return *this; }
    outiter<Container>& operator* () { return *this; }
    outiter<Container>& operator++ () { return *this; }
    outiter<Container>& operator++ (int) { return *this; }
};

template <typename Container, typename Iter>
inline outiter<Container, Iter> outinserter(Container& c, Iter it)
{
    return outiter<Container, Iter>(c, it);
}

class Tree {
    static size_t tags;
    char tag;
    Node* root;
    size_t n; 
public: 
    using key_type = int; 
    using value_type = int; 
    using key_compare = less<int>; 
    void swap(Tree& rgt)
    {
        using std::swap;
        swap(tag, rgt.tag);
        swap(root, rgt.root);
        swap(n, rgt.n);
    }
    static int Count;
    myiter Insert(const int& k, myiter where) {
        return insert(k, where).first;
    }
    size_t H() { return (root ? root->h : 0); }
    void Display(int = 1);
    myiter begin()const;
    myiter end()const { return myiter(nullptr); }
    void clear() { n = 0; delete root; root = nullptr; }
    pair<myiter, bool> insert(int, myiter = myiter(nullptr));
    Tree() : tag(static_cast<char>('A' + tags++)), root(nullptr), n(0) { }
    Tree(int N) : tag(static_cast<char>('A' + tags++)), root(nullptr), n(0) {
        clear();
        Count = 0;
        for (int i = 0; i < N; ++i) {
            int num = rand() % 16;
            Tree::Count += insert(num).second;
        }
    }
    int size() { return n; }
    ~Tree() { delete root; }
    myiter find(int)const;
    Tree(const Tree& rgt) : Tree() {
        for (auto x = rgt.begin(); x != rgt.end(); ++x) insert(*x);
    }
    Tree(Tree&& rgt) : Tree() { swap(rgt); }
    template<class MyIt> 
    Tree(MyIt first, MyIt last) : Tree() {
        for (; first != last; ++first) insert(*first);
    }
    Tree(list<int> data) : Tree() {
        for (auto x = data.begin(); x != data.end(); ++x) {
            insert(*x);
        }
    }
    Tree& operator=(const Tree& rgt)
    {
        Tree temp;
        for (auto x : rgt) temp.insert(x);
        swap(temp);
        return *this;
    }
    Tree& operator=(Tree&& rgt)
    {
        swap(rgt); return *this;
    }
    Tree& operator |= (const Tree&);
    Tree operator | (const Tree& rgt) const
    {
        Tree result(*this); return (result |= rgt);
    }
    Tree& operator &= (const Tree&);
    Tree operator & (const Tree& rgt) const
    {
        Tree result(*this); return (result &= rgt);
    }
    Tree& operator -= (const Tree&);
    Tree operator - (const Tree& rgt) const
    {
        Tree result(*this); return (result -= rgt);
    }
    Tree& operator ^= (const Tree&);
    Tree operator ^ (const Tree& rgt) const
    {
        Tree result(*this); return (result ^= rgt);
    }
    void showme() {
        cout << tag << ": { ";

        if (root) {
            cout << root->key << ' ';
            outnode(root);
        }

        cout << '}' << endl;
    }
    void outnode(Node* root) {
        if (root->L[0]) {
            cout << root->L[0]->key << ' ';
            outnode(root->L[0]);
        }
        if (root->L[1]) {
            cout << root->L[1]->key << ' ';
            outnode(root->L[1]);
        }
    }
    void Merge(Tree last, Tree& res) {
        res.clear();
        merge(begin(), end(), last.begin(), last.end(), outinserter(res, myiter(nullptr)));
    }
    void subst(int p, Tree last, Tree& res) {
        res.clear();
        auto qt = begin();
        int i = 0;

        while (i < p && qt != end()) {
            res.insert(*qt);
            ++qt;
            ++i;
        }
        for (auto st = last.begin(); st != last.end(); ++st) {
            res.insert(*st);
        }
        for (auto st = qt; st != end(); ++st) {
            res.insert(*st);
        }
    }
    void change(int p, Tree last, Tree& res) {
        res.clear();
        auto qt = begin();
        int i = 0;
        
        while (i < p && qt != end()) {
            res.insert(*qt);
            ++qt;
            ++i;
        }
        
        for (auto st = last.begin(); st != last.end(); ++st) {
            res.insert(*st);
            ++qt;
        }
        
        for (auto st = qt; st != end(); ++st) {
            res.insert(*st);
        }
    }
};

int Tree::Count;

myiter Tree::begin()const {
    MyStack St;
    Node* p(root);
    if (p) {
        while (p->L[0]) {
            St.push(make_pair(p, 0));
            p = p->L[0];
        }
    }
    return myiter(p, move(St));
}

myiter& myiter::operator++()
{
    if (!Ptr) { 
        return *this; 
    }
    if (Ptr->L[1]) { 
        St.push(make_pair(Ptr, 1));
        Ptr = Ptr->L[1];
        while (Ptr->L[0]) { 
            St.push(make_pair(Ptr, 0));
            Ptr = Ptr->L[0];
        }
    }
    else {
        pair<Node*, int> pp(Ptr, 1);
        while (!St.empty() && pp.second) { pp = St.top(); St.pop(); }
        if (pp.second) { 
            Ptr = nullptr;
        }
        else Ptr = pp.first; 
    }
    return (*this);
}

const int FIRSTROW = 0,
FIRSTCOL = 60,	
MAXCOL = 120,
OFFSET[] = { 60, 23, 12, 6, 3, 2, 1 },
MAXROW = FIRSTROW + 9, 
MAXOUT = FIRSTROW + 6,
SHIFT = 2;
string SCREEN[MAXROW]; 

void clrscr(int f = 1)
{
    for (auto i = 0; i < MAXROW; ++i) {
        
        SCREEN[i] = "";
        SCREEN[i].resize(MAXCOL + 20, '.');
    }
    if (f) system("cls");
}

void showscr()
{
    for (auto i = 0; i < MAXROW; ++i) {
        SCREEN[i].resize(MAXCOL, '.');
        cout << SCREEN[i] << '\n';
    }
}

int setval(string& s, int pos, int val) {
    string t(to_string(val));
    for (auto p : t) s[pos++] = p;
    return t.size();
}

void Tree::Display(int first)
{
    clrscr(first); 
    SCREEN[0] = "BSTh (H=" + to_string(H()) + " n=" + to_string(n) + ") --------->";
    if (root) { 
        SCREEN[0].resize(MAXCOL, '.');
        root->Display(0, FIRSTCOL, 1);
    }
    else SCREEN[0] += "<Empty!>";
    showscr();
}

void Node::Display(int row, int col, int depth)
{
    if ((row > MAXROW) || (col < 0) || (col > MAXCOL)) return;
    if (row > MAXOUT) {
        SCREEN[row].replace(col, 3, "+++");
        return;
    }
    
    try {
        setval(SCREEN[row], col, key);
        setval(SCREEN[row + 1], col, h);
    }
    catch (exception& e) {
        cout << e.what() << key << ' ' << row << ' ' << col << endl;
        cin.get();
    }
    catch (...) { cout << "Unknown error\n"; cin.get(); }
    
    if (L[0]) L[0]->Display(row + 1, col - OFFSET[depth], depth + 1);
    if (L[1]) L[1]->Display(row + 1, col + OFFSET[depth], depth + 1);
}
myiter Tree::find(int k)const
{
    Node* p(root);
    while (p && p->key != k) p = p->L[p->key > k];
    return myiter(p);
}

pair<myiter, bool> Tree::insert(int k, myiter where)
{
    Node* p(root), * q(nullptr);
    int a{ 0 };
    MyStack St;
    //===== Инициализация =====
    if (!where.Ptr) { 
        if (!root) { 
            root = new Node(k); 
            n = 1;		
            return make_pair(myiter(root, move(St)), true);         
        }
    }
    else {  
        p = where.Ptr;
        St = move(where.St);
    }

    //===== Поиск места вставки ===== //
    while (p) {
        a = k > p->key ? 1 : 0;
        St.push(make_pair(p, a));
        q = p->L[a];
        if (q) { 
            p = q; 
        }
        else {
            p->L[a] = q = new Node(k);  
            ++n;
            break;
        }
    }

    int b_old{ 0 };
    while (!St.empty())
    {
        auto pa = St.top(); St.pop();
        p = pa.first; a = pa.second;
        int b(p->balancefactor());
        if (b) {
            if ((b == 2) || (b == -2))
            { 
                --p->h;
                b /= 2;
                if (b == b_old) {	
                    p->L[a] = q->L[1 - a];
                    q->L[1 - a] = p;
                    if (p == root)p = root = q;
                    else St.top().first->L[St.top().second] = p = q;
                    p->fixheight();
                    break;
                }
                else {		
                    Node* r(q->L[1 - a]);
                    p->L[a] = r->L[1 - a];
                    q->L[1 - a] = r->L[a];
                    r->L[1 - a] = p;
                    r->L[a] = q;
                    
                    if (p == root) p = root = r;
                    else St.top().first->L[St.top().second] = p = r;
                    p->fixheight();
                    break;
                }
            }
            b_old = b; 
            p->fixheight();
            q = p;
        }
        else break; 
    }
    return make_pair(myiter(p, move(St)), true);
}

Tree& Tree::operator |= (const Tree& rgt) {
    Tree temp;
    set_union(begin(), end(), rgt.begin(), rgt.end(), outinserter(temp, myiter(nullptr)));
    swap(temp);
    return *this;
}

Tree& Tree::operator &= (const Tree& rgt) {
    Tree temp;
    set_intersection(begin(), end(), rgt.begin(), rgt.end(), outinserter(temp, myiter(nullptr)));
    swap(temp);
    return *this;
}

Tree& Tree::operator -= (const Tree& rgt) {
    Tree temp;
    set_difference(begin(), end(), rgt.begin(), rgt.end(), outinserter(temp, myiter(nullptr)));
    swap(temp);
    return *this;
}
Tree& Tree::operator ^= (const Tree& rgt) {
    Tree temp;
    set_symmetric_difference(begin(), end(), rgt.begin(), rgt.end(), outinserter(temp, myiter(nullptr)));
    swap(temp);
    return *this;
}
size_t Tree::tags = 0;





using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
	system("chcp 1251");
	srand(time(nullptr));

	int N = 16;
	Tree home(rand()%N+1),
		 second(rand()%N+1),
		 third(rand()%N+1),
		 fourth(rand()%N+1),
		 fifth(rand()%N+1),
		 res1, res2, res3, res;

	home.Display();
	cout << "\nSequence in the form of a tree (further will be in the form of a sequence) ... ";
	home.showme();
	cin.get();
	system("cls");



	home.showme();
	second.showme();
	third.showme();
	fourth.showme();
	fifth.showme(); cout << endl;

	res1 = home | second;
	cout << "A | B = "; res1.showme();
	res2 = third | fourth;
	cout << "C | D = "; res2.showme();
	res3 = res1 - res2;
	cout << "(A | B) - (C | D) = "; res3.showme();
	res = res3 ^ fifth;
	cout << "(A | B) - (C | D) ^ E = "; res.showme();

	cout << "\nGraphical representation in progress... ";
	cin.get();
	res.Display();
	cout << "Result: "; res.showme();
	cin.get();

	list<int> test = {6, 7, 9, 2, 3};
	Tree merge_tree(test), resx;
	home.Merge(merge_tree, resx);
	resx.Display();
	cout << "Operation Merge\n";
	resx.showme();
	cin.get();

	Tree subst_tree({3, 4, 0}), res4;
	home.subst(2, subst_tree, res4);
	res4.Display();
	cout << "Operation Subst\n";
	res4.showme();
	cin.get();

	Tree change_tree({2, 8, 7}), res5;
	home.change(3, change_tree, res5);
	res5.Display();
	cout << "Operation Change\n";
	res5.showme();

	cout << "\n ==== The end ====\n";
	cin.get();
	return 0;
}



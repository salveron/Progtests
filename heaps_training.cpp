#include <iostream>
#include <cassert>
#include <memory>

using namespace std;

struct CEntity{
    int degree;
    unsigned int id;
    unsigned int revenue;
    CEntity * parent;
    CEntity * left_sibling;
    CEntity * right_sibling;
    CEntity * right_son;

    CEntity(unsigned int id, unsigned int revenue){
        degree =  0;
        this->id = id;
        this->revenue = revenue;
        parent = left_sibling = right_sibling = right_son = nullptr;
    }
};

CEntity * mergeTrees(CEntity * root_1, CEntity * root_2){
    if (!root_1 || !root_2)
        return nullptr;

    if (root_1->degree != root_2->degree)
        return nullptr;

    if (root_1->revenue > root_2->revenue)
        swap(root_1, root_2);

    root_2->parent = root_1;
    root_2->left_sibling = root_1->right_son;
    if (root_1->right_son) root_1->right_son->right_sibling = root_2;
    root_1->right_son = root_2;
    root_2->right_sibling = nullptr;
    root_1->degree++;

    return root_1;
}

class CChain{
public:
    int chain_id;
    CEntity * root;
    CEntity * min;

    CChain(){
        root = min = nullptr;
    }
    CChain(int num){
        chain_id = num;
        root = min = nullptr;
    }
    ~CChain(){
        deleteHeap(root);
    };

    void insertEntity(unsigned int id, unsigned int revenue){
        auto * new_entity =  new CEntity(id, revenue);
        root = mergeHeaps(new_entity);
    }

    CEntity * mergeHeaps(CEntity * target){
        if (!target) return root;

        CEntity * result = nullptr;
        CEntity * tmp = result;
        CEntity * subtree;
        CEntity * subtrees [3];
        CEntity * carry = nullptr;
        int not_nulls = 2;
        int actual_degree = 0;

        bool target_is_reversed = (target && target->left_sibling && target->degree > target->left_sibling->degree);
        if (target_is_reversed){
            while(target->left_sibling)
                target = target->left_sibling;
        }

        while(not_nulls >= 2){
            not_nulls = 0;
            int num_of_terms = 0;

            if (root){
                not_nulls++;
                subtree = root;
                if (subtree->degree == actual_degree){
                    subtrees[num_of_terms] = subtree;
                    num_of_terms++;

                    root = root->left_sibling;
                }
            }
            if (target){
                not_nulls++;
                subtree = target;
                if (subtree->degree == actual_degree){
                    subtrees[num_of_terms] = subtree;
                    num_of_terms++;

                    if(target_is_reversed)
                        target = target->left_sibling;
                    else target = target->right_sibling;
                }
            }
            if (carry){
                not_nulls++;
                subtrees[num_of_terms] = carry;
                num_of_terms++;
                carry = nullptr;
            }
            if (num_of_terms == 3){
                if (!result) {
                    result = subtrees[2];
                    tmp = result;
                } else{
                    tmp->left_sibling = subtrees[2];
                    tmp->left_sibling->right_sibling = tmp;
                    tmp = tmp->left_sibling;
                }
                carry = mergeTrees(subtrees[0], subtrees[1]);
            } else if (num_of_terms == 2){
                carry = mergeTrees(subtrees[0], subtrees[1]);
            } else if (num_of_terms == 1){
                if (!result) {
                    result = subtrees[0];
                    tmp = result;
                } else{
                    tmp->left_sibling = subtrees[0];
                    tmp->left_sibling->right_sibling = tmp;
                    tmp = tmp->left_sibling;
                }
            }
            actual_degree++;
        }
        while (root){
            if (!tmp) result = tmp = root;
            tmp->left_sibling = root;
            tmp->left_sibling->right_sibling = tmp;
            tmp = tmp->left_sibling;

            root = root->left_sibling;
        }
        while (target){
            if (!tmp) result = tmp = target;
            tmp->left_sibling = target;
            tmp->left_sibling->right_sibling = tmp;
            tmp = tmp->left_sibling;

            if (target_is_reversed)
                target = target->left_sibling;
            else target = target->right_sibling;
        }

        //Setting the minimum pointer
        tmp = result;
        min = result;
        while(tmp){
            if (tmp->revenue < min->revenue)
                min = tmp;
            tmp = tmp->left_sibling;
        }

        return result;
    }

    CEntity * extractMin(){
        if (!root) return nullptr;
        CEntity * result = min;

        if (root == min) {
            //moving the root
            root = root->left_sibling;

            //resetting the min pointer
            CEntity * tmp = root;
            min = tmp;
            while(tmp){
                if (tmp->revenue < min->revenue || (tmp->revenue == min->revenue && tmp->id < min->id))
                    min = tmp;
                tmp = tmp->left_sibling;
            }
        }

        if (!result->left_sibling && !result->right_sibling){ //if min is the root of a single tree
            min = root = result->right_son;
            if (!root) return result;

            while (root->left_sibling){
                //reversing the heap and deleting every entity's parent
                root->parent = nullptr;
                root->right_sibling = root->left_sibling;
                root->left_sibling = nullptr;
                if (root->right_sibling->revenue < min->revenue)
                    min = root->right_sibling;
                root = root->right_sibling;
            }
            root->left_sibling = root->right_sibling;
            result->right_son = root->parent = root->right_sibling = nullptr;
            return result;
        }

        //resetting the sibling pointers
        if (result->left_sibling) result->left_sibling->right_sibling = result->right_sibling;
        if (result->right_sibling) result->right_sibling->left_sibling = result->left_sibling;

        mergeHeaps(result->right_son);

        return result;
    }

    void deleteHeap(CEntity * heap_root){
        if (!heap_root) return;
        deleteHeap(heap_root->right_son);
        deleteHeap(heap_root->left_sibling);
        delete heap_root;
    }

private:

};

class CHolding {
public:
    CHolding(){
        size = 10000;
        number_of_elements = 0;
        binary_heap = new CChain [size];
    }
    ~CHolding(){
        delete [] binary_heap;
    }

    void Add(int chain, unsigned int id, unsigned int revenue){
        CChain * new_chain = findChain(chain);
        if (!new_chain){
            addNewChain(chain);
            binary_heap[number_of_elements - 1].insertEntity(id, revenue);
            bubbleUp(number_of_elements - 1);
        } else new_chain->insertEntity(id, revenue);
    }

    bool Remove(int chain, unsigned int &id){
        CChain * found_chain = findChain(chain);
        if (!found_chain) return false;
        CEntity * extracted = found_chain->extractMin();
        if (!extracted) return false;
        id = extracted->id;
        return true;
    }

    /*bool Remove(unsigned int &id){

    }

    void Merge(int dstChain, int srcChain){

    }

    bool Merge(void){

    }*/

private:
    CChain * binary_heap;
    long long int size;
    long long int number_of_elements;

    void swapChains(long long int a, long long int b){
        CChain tmp = binary_heap[a];
        binary_heap[a] = binary_heap[b];
        binary_heap[b] = tmp;
    }

    void bubbleUp(long long int num){
        if (!binary_heap[num].min) return;
        while(num != 0){
            CChain parent = binary_heap[(num - 1) / 2];
            if (parent.min && parent.min->revenue <= binary_heap[num].min->revenue) return;
            swapChains((num - 1) / 2, num);
            num = (num - 1) / 2;
        }
    }

    void addNewChain(int chain_id){ // inserting into the min binary heap
        if (number_of_elements == size){
            size *= 2;
            auto * new_heap = new CChain [size];
            for (long long int i = 0; i < number_of_elements; i++)
                new_heap[i] = binary_heap[i];
            delete [] binary_heap;
            binary_heap = new_heap;
        }

        binary_heap[number_of_elements].chain_id = chain_id;
        number_of_elements++;
    }

    CChain * findChain(int chain_id){//todo - linear complexity ATM
        for (long long int i = 0; i < number_of_elements; i++)
            if (binary_heap[i].chain_id == chain_id)
                return &binary_heap[i];
        return nullptr;
    }

    /*void deleteTree(CChain * tree_root){
        if (!tree_root->left && !tree_root->right){
            tree_root->deleteHeap(tree_root->root);
            delete tree_root;
            return;
        }
        deleteTree (tree_root->left);
        deleteTree (tree_root->right);
    }*/
};

int main() {
    bool res;
    unsigned int id;

    //Ukazkovy vstup #1
    //------------------------------------------------------------------------------------------------------------------
    CHolding f1;
    f1.Add(7, 2, 9);
    f1.Add(12, 4, 4);
    f1.Add(6, 15, 2);
    f1.Add(6, 9, 3);

    res = f1.Remove(12, id);
    // res = true, id = 4
    assert(res && id == 4);

    res = f1.Remove(12, id);
    // res = false, id = N/A
    assert(!res);

    res = f1.Remove(6, id);
    // res = true, id = 15
    assert(res && id == 15);

    res = f1.Remove(6, id);
    // res = true, id = 9
    assert(res && id == 9);

    res = f1.Remove(6, id);
    // res = false, id = N/A
    assert(!res);

    //Ukazkovy vstup #2
    //------------------------------------------------------------------------------------------------------------------
    /*CHolding f2;
    f2.Add(4, 2, 2);
    f2.Add(1, 4, 3);
    f2.Add(8, 9, 8);
    res = f2.Remove(id);
    // res = true, id = 2
    assert(res && id == 2);
    res = f2.Remove(id);
    // res = true, id = 4
    assert(res && id == 4);

    //Ukazkovy vstup #3
    //------------------------------------------------------------------------------------------------------------------
    CHolding f3;
    f3.Add(10, 101, 9);
    f3.Add(10, 102, 8);
    f3.Add(10, 103, 7);
    f3.Add(10, 104, 6);
    f3.Add(10, 105, 5);
    f3.Add(20, 201, 9);
    f3.Add(20, 202, 8);
    f3.Add(20, 203, 7);
    f3.Add(20, 204, 6);
    f3.Add(20, 205, 5);
    f3.Add(30, 301, 9);
    f3.Add(30, 302, 8);
    f3.Add(30, 303, 7);
    f3.Add(30, 304, 6);
    f3.Add(30, 305, 5);
    res = f3.Remove(id);
    // res = true, id = 105
    assert(res && id == 105);
    res = f3.Remove(id);
    // res = true, id = 205
    assert(res && id == 205);
    res = f3.Remove(id);
    // res = true, id = 305
    assert(res && id == 305);
    res = f3.Remove(id);
    // res = true, id = 104
    assert(res && id == 104);
    res = f3.Remove(id);
    // res = true, id = 204
    assert(res && id == 204);
    res = f3.Remove(id);
    // res = true, id = 304
    assert(res && id == 304);
    res = f3.Remove(id);
    // res = true, id = 103
    assert(res && id == 103);
    res = f3.Remove(id);
    // res = true, id = 203
    assert(res && id == 203);
    res = f3.Remove(id);
    // res = true, id = 303
    assert(res && id == 303);
    res = f3.Remove(id);
    // res = true, id = 102
    assert(res && id == 102);
    res = f3.Remove(id);
    // res = true, id = 202
    assert(res && id == 202);
    res = f3.Remove(id);
    // res = true, id = 302
    assert(res && id == 302);*/

/*
    //Ukazkovy vstup #4
    //------------------------------------------------------------------------------------------------------------------
    CHolding f4;
    f4.Add(10, 101, 9);
    f4.Add(10, 102, 8);
    f4.Add(10, 103, 7);
    f4.Add(10, 104, 6);
    f4.Add(10, 105, 5);
    f4.Add(20, 201, 9);
    f4.Add(20, 202, 8);
    f4.Add(20, 203, 7);
    f4.Add(20, 204, 6);
    f4.Add(20, 205, 5);
    f4.Add(30, 301, 9);
    f4.Add(30, 302, 8);
    f4.Add(30, 303, 7);
    f4.Add(30, 304, 6);
    f4.Add(30, 305, 5);
    res = f4.Remove(30, id);
    // res = true, id = 305
    res = f4.Remove(20, id);
    // res = true, id = 205
    res = f4.Remove(10, id);
    // res = true, id = 105
    f4.Merge(30, 10);
    res = f4.Remove(10, id);
    // res = false, id = N/A
    res = f4.Remove(20, id);
    // res = true, id = 204
    res = f4.Remove(30, id);
    // res = true, id = 104
    res = f4.Remove(id);
    // res = true, id = 304
    res = f4.Remove(id);
    // res = true, id = 203
    res = f4.Remove(id);
    // res = true, id = 103
    res = f4.Remove(id);
    // res = true, id = 303
    res = f4.Remove(id);
    // res = true, id = 202
    res = f4.Remove(id);
    // res = true, id = 102
    res = f4.Remove(id);
    // res = true, id = 302
    res = f4.Remove(id);
    // res = true, id = 201
    res = f4.Remove(id);
    // res = true, id = 101
    res = f4.Remove(id);
    // res = true, id = 301
    res = f4.Remove(id);
    // res = false, id = N/A
    res = f4.Remove(id);
    // res = false, id = N/A


    //Ukazkovy vstup #5
    //------------------------------------------------------------------------------------------------------------------
    CHolding f5;
    f5.Add(10, 333, 5);
    f5.Add(20, 444, 2);
    f5.Add(10, 222, 6);
    f5.Add(20, 555, 8);
    res = f5.Remove(10, id);
    // res = true, id = 333
    res = f5.Remove(id);
    // res = true, id = 444
    f5.Merge(20, 10);
    res = f5.Remove(10, id);
    // res = false, id = N/A
    res = f5.Remove(20, id);
    // res = true, id = 222


    //Ukazkovy vstup #6
    //------------------------------------------------------------------------------------------------------------------
    CHolding f6;
    f6.Add(10, 1, 7);
    f6.Add(20, 1, 7);
    f6.Add(30, 1, 7);
    res = f6.Merge();
    // res = true
    res = f6.Remove(20, id);
    // res = false, id = N/A
    res = f6.Remove(30, id);
    // res = true, id = 1
    res = f6.Remove(30, id);
    // res = false, id = N/A
    res = f6.Remove(10, id);
    // res = true, id = 1
    res = f6.Remove(10, id);
    // res = true, id = 1
    res = f6.Remove(10, id);
    // res = false, id = N/A


    //Ukazkovy vstup #7
    //------------------------------------------------------------------------------------------------------------------
    CHolding f7;
    f7.Add(1, 1, 1);
    f7.Add(2, 2, 1);
    f7.Add(3, 3, 1);
    res = f7.Merge();
    // res = true
    res = f7.Merge();
    // res = true
    res = f7.Merge();
    // res = false
    res = f7.Remove(id);
    // res = true, id = 1
    res = f7.Remove(id);
    // res = true, id = 2
    res = f7.Remove(id);
    // res = true, id = 3
    res = f7.Remove(id);
    // res = false, id = N/A
*/
    return 0;
}
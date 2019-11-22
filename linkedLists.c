#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**This program is the simple linked lists training. I created some functions to store employees and their bosses.*/

typedef struct TEmployee{
    struct TEmployee * m_Next;
    struct TEmployee * m_Bak;
    char * m_Name;
} TEMPLOYEE;

#endif /* __PROGTEST__ */

TEMPLOYEE * newEmployee ( const char * name, TEMPLOYEE * next );
void insertCopies ( TEMPLOYEE * prev );
void setM_BakPointers ( TEMPLOYEE * src );

/**
 * This function clones a LL. It firstly inserts copies of all the nodes and sets the boss pointers. Then it divides the
 * result into two separate LLs and returns a copy.
 *
 * @param[in] src is the root of the LL to be cloned.
 * */
TEMPLOYEE * cloneList ( TEMPLOYEE * src ){
    if ( ! src ) return NULL;

    insertCopies(src);
    setM_BakPointers(src);

    TEMPLOYEE * tmp = src->m_Next;
    TEMPLOYEE * copy = src->m_Next;

    while( src->m_Next->m_Next ){
        src->m_Next = src->m_Next->m_Next;
        tmp->m_Next = tmp->m_Next->m_Next;
        src = src->m_Next;
        tmp = tmp->m_Next;
    }

    src->m_Next = NULL;
    tmp->m_Next = NULL;

    return copy;
}

TEMPLOYEE * newEmployee  ( const char * name, TEMPLOYEE * next ){
    TEMPLOYEE * new_first = (TEMPLOYEE*) malloc (sizeof(TEMPLOYEE));
    new_first->m_Next = next;
    new_first->m_Name = (char*) malloc ((strlen(name) + 1) * sizeof(char));
    strncpy( new_first->m_Name, name, strlen(name) + 1 ); //Deep copy of the name
    new_first->m_Bak = NULL;
    return new_first;
}

void insertCopies ( TEMPLOYEE * prev ){
    while ( prev ){
        TEMPLOYEE * new_next = (TEMPLOYEE*) malloc (sizeof(TEMPLOYEE));
        new_next->m_Next = prev->m_Next;
        new_next->m_Name = (char*) malloc ((strlen(prev->m_Name) + 1) * sizeof(char));
        strncpy( new_next->m_Name, prev->m_Name, strlen(prev->m_Name) + 1 );
        new_next->m_Bak = NULL;
        prev->m_Next = new_next;
        prev = new_next->m_Next;
    }
}

void setM_BakPointers ( TEMPLOYEE * src ){

    while ( src ){
        if ( src->m_Bak ) src->m_Next->m_Bak = src->m_Bak->m_Next;
        src = src->m_Next->m_Next;
    }
}

void freeList ( TEMPLOYEE * src ){
    TEMPLOYEE * tmp;

    while ( src ){
        tmp = src->m_Next;
        free(src->m_Name);
        src->m_Name = NULL;
        free(src);
        src = tmp;
    }
}

#ifndef __PROGTEST__

void printList ( TEMPLOYEE * src ){

    while ( src ){
        printf("%s\n", src->m_Name);
        src = src->m_Next;
    }
    printf("\n");
}

int main ( int argc, char * argv [] ){

    TEMPLOYEE * a = NULL;
    TEMPLOYEE * b = NULL;
    char tmp[100];
    char peter[6] = "Peter";
    char john[5] = "John";
    char joe[4] = "Joe";
    char maria[6] = "Maria";

    a = newEmployee ( peter, a );
    a = newEmployee ( john, a );
    a = newEmployee ( joe, a );
    a = newEmployee ( maria, a );
    a -> m_Bak = a -> m_Next;
    a -> m_Next -> m_Next -> m_Bak = a -> m_Next -> m_Next -> m_Next;
    a -> m_Next -> m_Next -> m_Next -> m_Bak = a -> m_Next;

    printList(a);
    printf("%s\n\n", a->m_Bak->m_Name);
    printf("%s\n\n", a->m_Next->m_Next->m_Bak->m_Name);
    printf("%s\n\n", a->m_Next->m_Next->m_Next->m_Bak->m_Name);

    b = cloneList ( a );

    printList(a);
    printList(b);

    strncpy ( tmp, "Moe", sizeof ( tmp ) );
    a = newEmployee ( tmp, a );

    strncpy ( tmp, "Victoria", sizeof ( tmp ) );
    a = newEmployee ( tmp, a );

    printList(a);

    strncpy ( tmp, "Peter", sizeof ( tmp ) );
    a = newEmployee ( tmp, a );
    b -> m_Next -> m_Next -> m_Next -> m_Bak = b -> m_Next -> m_Next;

    printList(a);
    printList(b);

    freeList(a);
    freeList(b);

    return 0;
}
#endif /* __PROGTEST__ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Test linked list node deletion using pointer to pointer
// see: http://coolshell.cn/articles/8990.html
typedef struct point {
    int x, y, z;
}point;

typedef struct node {
    struct node* next;
    // Here use pointer to pointer just for test remove function
    point* pos;
}node;

typedef int (*remove_func)(node *p);

static node* new_node(const point* pt) {
    node *p = (node*) malloc(sizeof(node));
    if(!p)
        return NULL;

    p->pos = (point*) malloc(sizeof(point));
    if(!p->pos) {
        free(p);
        return NULL;
    }
    memcpy(p->pos, pt, sizeof(point));
    return p; 
}

static void dump(node* head) {
    printf("dump list:------------------\n");
    while(head) {
        if(head->pos)
            printf("(%d, %d, %d)\n", head->pos->x, head->pos->y, head->pos->z);
        head = head->next;
    }
    printf("dump end.-------------------\n");
}

// remove and destroy the right node, return new head if changed.
static node* remove_if_old(node* head, remove_func fn) {
    for(node* prev = 0, *cur = head; cur; ) {
        node *next = cur->next;
        if(fn(cur)) {
            if(prev) 
                prev->next = next; 
            else
                head = next;
            free(cur);
        } else {
            prev = cur;
        }
        cur = next;
    }
    return head;
}

// remove and destroy the right node using pointer to pointer, return new head if changed.
static void remove_if(node** head, remove_func fn) {
    for(node** pp = head; *pp; ) {
        node* entry = *pp;
        if(fn(entry)) {
            *pp = entry->next;
            free(entry);
        } else {
            pp = &entry->next;
        }
    } 
}

static node* fill_list() {
    int count = rand() % 5 + 5;
    node *head = 0;
    for(int i = 0; i < count; i++){
        point pt;
        if(rand() % 3 == 0) {
            pt.x = pt.y = pt.z = 0;
        } else { 
            pt.x = rand() % 10;
            pt.y = rand() % 10;
            pt.z = rand() % 10;    
        }
        node *p = new_node(&pt);
        if(!p)
            return head;
        p->next = head;
        head = p;
    }
    return head;
}

static int remove_all(node* entry) {
    if(entry->pos){
        free(entry->pos);
        entry->pos = 0;
    }
    return 1;
}

static int remove_zero_point(node* entry) {
    if(entry->pos) {
        if(entry->pos->x == 0 &&
                entry->pos->y == 0 &&
                entry->pos->z == 0) {
            free(entry->pos);
            entry->pos = 0;
            return 1;
        }
    }
    return 0;
}

static void remove_test_old() {
    node* head = fill_list();
    dump(head);
    head = remove_if_old(head, remove_zero_point);
    dump(head);
    head = remove_if_old(head, remove_all);
    dump(head);
}

static void remove_test_new() {
    node* head = fill_list();
    dump(head);
    remove_if(&head, remove_zero_point);
    dump(head);
    remove_if(&head, remove_all);
    dump(head);

}

int main() {
    srand(time(NULL));
    remove_test_old();
    remove_test_new();
}

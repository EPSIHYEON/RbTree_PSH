#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

void inorder_recursive(const rbtree *t,node_t *x, key_t *arr, int *index , const size_t n);
void rb_insert_fixup(rbtree *t, node_t *z);
void left_rotate(rbtree *t, node_t *x);
void right_rotate(rbtree *t, node_t *x);
void rb_transplant(rbtree *t, node_t* u, node_t* v);
node_t *Tree_Minimum(rbtree* t, node_t *x);
node_t *Tree_Maximum(rbtree* t, node_t *x);
void rb_delete_fixup(rbtree *t, node_t *x);
void remove_all_nodes(rbtree *t, node_t *x);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = (node_t *)calloc(1, sizeof(node_t)); //nil 노드 생성
  nil->color = RBTREE_BLACK;

  nil->parent = nil;
  nil->left = nil;
  nil->right = nil;

  p->nil = p->root = nil; // 처음 상태는 nil 노드 그 자체(가상의 비어있는 노드)
  return p;
}


void remove_all_nodes(rbtree *t, node_t *x){
  if(x == t->nil){
    return;
  }
//후위순회
  remove_all_nodes(t, x->left);
  remove_all_nodes(t,x->right);

  free(x);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  remove_all_nodes(t, t->root);
  free(t->nil); //nil 노드도 free
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *x = t->root;
  node_t *y = t->nil;

  node_t *z = (node_t *)malloc(sizeof(node_t));
  z->color = RBTREE_RED;
  z->parent = t->nil;
  z->key = key;

  while(x != t->nil){
    y = x;
    if(z->key < x->key){
      x = x->left;
    }else{
      x = x->right;
    }
  }
  z->parent = y;
  if(y == t->nil){
    t->root = z;
  }
  else if(z->key < y->key){
    y->left = z;
  }else { y->right = z; }

  z->left = t->nil; //z는 끝이니까 끝에 nil 노드 붙임
  z->right = t->nil;

  z->color = RBTREE_RED;

  rb_insert_fixup(t, z);

  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *cur = t->root;

  while(cur != t->nil){
    if(cur->key == key){
      //printf("FIND KEY!");
      return cur;
    }
    else if(cur->key > key){
      cur = cur->left;
    }
    else{
      cur = cur->right;
    }
  }
  //printf("NOT FIND!");
  return NULL;
  //return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  while(x->left != t->nil){
    x = x->left;
  }
  return x;
  //return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  while(x->right != t->nil){
    x = x->right;
  }
  return x;
  //return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  // x = y가 사라졌을 때 다른 애들이 연결할 노드(y의 위치를 대체함) , y = p 대신 사라질 희생양 , p = 실제로 삭제되어야하지만 y의 값만 받아오고 삭제되지 않을 노드
  node_t *y = p;
  node_t *x;
  color_t y_original_color = y->color;

  if(p->left == t->nil){ //왼쪽 자식이 없을때
    x = p->right;

    rb_transplant(t,p,p->right); //p는 버리고, 그 자리에 p->right를 연결
  }
  else if(p->right == t->nil){ //왼쪽자식이 있고, 오른쪽 자식이 없을때
    x = p->left;

    rb_transplant(t, p, p->left); //p는 버리고, 그 자리에 p->left를 연결

  }
  else{//자식이 양쪽에 있으면, 자신보다 큰값 중 가장 작은 값을 골라서 와
    y = Tree_Minimum(t, p->right);
    y_original_color = y->color;
    x = y->right; //nil 노드이거나 y보다 큰 값임

    if(y != p->right){ //p가 y의 부모노드가 아니면,

      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }

    else{ //p가 y의 부모노드라면
      x->parent = y;

     }
      rb_transplant(t,p,y); //p의 위치에 y로 교체
      y->left = p->left; // y의 왼쪽이었던 서브트리를 p의 왼쪽에 둠 (대체노드랑 연결이 끊어지고 p의 위치에 y가 들어간 상태)
      y->left->parent = y; //왼쪽 자식이 없는 y
      y->color = p->color;

  }

   if (y_original_color == RBTREE_BLACK) {

        rb_delete_fixup(t, x);

    }
    t->nil->parent = t->nil;
    free(p);
    p=NULL;

  return 0;
}


int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int index = 0;
  inorder_recursive(t,t->root, arr, &index, n);

  return 0;
}

void inorder_recursive(const rbtree *t,node_t *x, key_t *arr, int *index , const size_t n){

  if(x == t->nil){
    return;
  }
  if(*index >= n){
    return;
  }
  inorder_recursive(t,x->left,arr, index,n);
  if(*index < n){
  arr[*index] = x->key;
  (*index)++; // ++ > * 우선순위
  }
  else{
    return;
  }

  inorder_recursive(t, x->right, arr, index,n );

}

void rb_insert_fixup(rbtree *t, node_t *z){
  node_t *uncle;
  while(z->parent != t->nil && z->parent->color == RBTREE_RED){
    if(z->parent == z->parent->parent->left) {//부모가 조상노드의 왼쪽일 때
      uncle = z->parent->parent->right; // uncle 은 삼촌 노드
      if(uncle->color == RBTREE_RED){
        uncle->color = RBTREE_BLACK; //삼촌과 부모는 BLACK으로
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED; //조상은 RED로 칠함
        z= z->parent->parent; // 조상의 조상 과 조상의 부모에 DOUBLERED 가 있는지 확인
      }
      else{ //unlce 이 BLACK일때
        if(z == z->parent->right){ //내가 오른쪽 자식일때
          z = z->parent;
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);

      }

    }
    else{
        uncle = z->parent->parent->left;
        if(uncle->color == RBTREE_RED){
          uncle->color = RBTREE_BLACK; //삼촌과 부모는 BLACK으로
          z->parent->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED; //조상은 RED로 칠함
          z= z->parent->parent; // 조상의 조상 과 조상의 부모에 DOUBLERED 가 있는지 확인
        }
        else{
          if (z == z->parent->left){
            z = z->parent; 
            right_rotate(t, z);
          }
          z->parent->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          left_rotate(t, z->parent->parent);
        }
    }
  }

  t->root->color = RBTREE_BLACK;
}

void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right;

  x->right = y->left; // y의 왼쪽 서브 트리를 x의 오른쪽 서브 트리로 회전한다

  if (y->left != t->nil){ // y의 왼쪽 서브 트리가 비어있지 않은 경우
    y->left->parent = x; // x는 y의 서브트리의 부모가 된다
  }
  y->parent = x->parent; //x의 부모가 y의 부모가 된다(x가 처음에는 더 위에 있었으니)
  if(x->parent == t->nil){ //x의 부모가 없었다면 x가 root였다는 것이므로
    t->root = y; //y가 root
  }
  else if(x == x->parent->left){ //x가 왼쪽 자식이었던 경우
    x->parent->left = y; // y는 왼쪽 자식이 된다
  }
  else{
    x->parent->right = y; //아니라면 x는 오른쪽 자식이었고, y를 오른쪽 자식이 된다
  }
  y->left = x; //**x를 y의 왼쪽 자식이 되게 한다** 최종 GOAL!
  x->parent = y;
}


void right_rotate(rbtree *t, node_t *x) {
    node_t *y = x->left;

    x->left = y->right; // y의 오른쪽 서브 트리를 x의 왼쪽 서브 트리로 만든다

    if (y->right != t->nil) { // y의 오른쪽 서브 트리가 비어있지 않은 경우
        y->right->parent = x; // x가 그 서브트리의 부모가 된다
    }

    y->parent = x->parent; // x의 부모가 y의 부모가 된다 (y가 이제 x 자리로 올라가니까)

    if (x->parent == t->nil) { // x의 부모가 없었다면 x가 root였다는 것이므로
        t->root = y; // y가 새로운 root
    } else if (x == x->parent->left) { // x가 왼쪽 자식이었던 경우
        x->parent->left = y; // y가 그 자리를 차지한다
    } else { // x가 오른쪽 자식이었던 경우
        x->parent->right = y;
    }

    y->right = x; // x를 y의 오른쪽 자식이 되게 한다. 최종 GOAL!
    x->parent = y; // x의 새로운 부모는 y가 된다
}

void rb_transplant(rbtree *t, node_t *u, node_t *v){ //u를 v로 교체
  if(u->parent == t->nil){ //u가 root 면
    t->root = v; //v로 root 변경
  }
  else if(u == u->parent->left){ //u가 왼쪽 자식이면
    u->parent->left = v; //왼쪽에 v의 서브트리를 u를 가진 서브트리를 빼고 u의 부모 노드에 붙임
  }
  else{ //u가 오른쪽 자식이면
    u->parent->right = v;//오른쪽에 v의 서브트리를 u를 가진 서브트리를 빼고 u의 부모 노드에 붙임
  }
        v->parent = u->parent;


}

node_t *Tree_Minimum(rbtree* t, node_t *x){
  while(x->left != t->nil){
    x = x->left;
  }
  return x;
}

node_t *Tree_Maximum(rbtree* t, node_t *x){
  while(x->right != t->nil){
    x = x->right;
  }
  return x;
}

void rb_delete_fixup(rbtree *t, node_t *x){ // fix하는 것은 y가 아닌 y의 자리에 들어간 x임
  node_t *w;
while(x != t->root && x->color == RBTREE_BLACK){ //x가 black 이고 x가 root가 아니면 실행
  if(x == x->parent->left){ //x가 왼쪽 자식일때
    w = x->parent->right;
    if(w->color == RBTREE_RED){ //CASE1 If 형제노드가 RED 일때
      w->color = RBTREE_BLACK;
      x->parent->color = RBTREE_RED;
      left_rotate(t, x->parent);
      w = x->parent->right;
    }
    if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ //CASE2 if 형제노드가 블랙이고 / 자식들이 둘다 black일때 (NIL 노드 포함)
      w->color = RBTREE_RED;
      x = x->parent;
    }
    else{ //형제노드가 black 일때
      if(w->right->color == RBTREE_BLACK){ //오른쪽이 블랙일때 //CASE3
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(t, w);
        w = x->parent->right;
      }
    w->color = x->parent->color; //CASE4 (오른쪽이 레드일때)
    x->parent->color = RBTREE_BLACK;
    w->right->color = RBTREE_BLACK;
    left_rotate(t,x->parent);
    x= t->root;
    }
  }
  else{//x가 오른쪽 자식일때
      w = x->parent->left;
    if(w->color == RBTREE_RED){ //CASE1 If 형제노드가 RED 일때 //CASE1
      w->color = RBTREE_BLACK;
      x->parent->color = RBTREE_RED;
      right_rotate(t, x->parent);
      w = x->parent->left;
    }
    if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ //CASE2 if 형제노드가 블랙이고 / 자식들이 둘다 black일때 (NIL 노드 포함)
      w->color = RBTREE_RED;
      x = x->parent;
    }
    else{ //형제노드가 black 일때
      if(w->left->color == RBTREE_BLACK){ //오른쪽이 블랙일때 //CASE3
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(t, w);
        w = x->parent->left;
      }
    w->color = x->parent->color; //CASE4
    x->parent->color = RBTREE_BLACK;
    w->left->color = RBTREE_BLACK;
    right_rotate(t,x->parent);
    x= t->root;
    }
  }
}
x->color = RBTREE_BLACK; // x가 red면 그냥 black으로 바꿔줌
}



// void rb_delete_fixup(rbtree *t, node_t *x) {
//     node_t *w;
//     while (x != t->root && x->color == RBTREE_BLACK) {
//         if (x == x->parent->left) { // x가 왼쪽 자식일 때
//             w = x->parent->right;
//             if (w->color == RBTREE_RED) { // Case 1
//                 w->color = RBTREE_BLACK;
//                 x->parent->color = RBTREE_RED;
//                 left_rotate(t, x->parent);
//                 w = x->parent->right;
//             }
//             if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // Case 2
//                 w->color = RBTREE_RED;
//                 x = x->parent;
//             } else {
//                 if (w->right->color == RBTREE_BLACK) { // Case 3
//                     w->left->color = RBTREE_BLACK;
//                     w->color = RBTREE_RED;
//                     right_rotate(t, w);
//                     w = x->parent->right;
//                 }
//                 // Case 4
//                 w->color = x->parent->color;
//                 x->parent->color = RBTREE_BLACK;
//                 w->right->color = RBTREE_BLACK;
//                 left_rotate(t, x->parent);
//                 x = t->root; // 루프 종료
//             }
//         } else { // x가 오른쪽 자식일 때 (대칭적인 케이스)
//             w = x->parent->left;
//             if (w->color == RBTREE_RED) { // Case 1 (대칭)
//                 w->color = RBTREE_BLACK;
//                 x->parent->color = RBTREE_RED;
//                 right_rotate(t, x->parent);
//                 w = x->parent->left;
//             }
//             if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) { // Case 2 (대칭)
//                 w->color = RBTREE_RED;
//                 x = x->parent;
//             } else {
//                 // [버그 수정] Case 3 조건 및 로직 수정
//                 if (w->left->color == RBTREE_BLACK) { // Case 3 (대칭)
//                     w->right->color = RBTREE_BLACK;
//                     w->color = RBTREE_RED;
//                     left_rotate(t, w);
//                     w = x->parent->left;
//                 }
//                 // [버그 수정] Case 4 로직 수정
//                 // Case 4 (대칭)
//                 w->color = x->parent->color;
//                 x->parent->color = RBTREE_BLACK;
//                 w->left->color = RBTREE_BLACK;
//                 right_rotate(t, x->parent);
//                 x = t->root; // 루프 종료
//             }
//         }
//     }
//     x->color = RBTREE_BLACK;
// }


// int rbtree_erase(rbtree *t, node_t *p) {
//     node_t *y = p;
//     node_t *x;
//     node_t *x_parent;
//     color_t y_original_color = y->color;

//     if (p->left == t->nil) {
//         x = p->right;
//         x_parent = p->parent;
//         rb_transplant(t, p, p->right);
//     } else if (p->right == t->nil) {
//         x = p->left;
//         x_parent = p->parent;
//         rb_transplant(t, p, p->left);
//     } else {
//         y = Tree_Minimum(t, p->right);
//         y_original_color = y->color;
//         x = y->right;
//         if (y->parent == p) { //y가 p의 직계자식일때
//             if(x != t->nil) x->parent = y;
//             x_parent = y;
//         } else {
//             x_parent = y->parent;
//             rb_transplant(t, y, y->right); //y가 p의 손자 이상일때
//             y->right = p->right;
//             y->right->parent = y;
//         }
//         rb_transplant(t, p, y);
//         y->left = p->left;
//         y->left->parent = y;
//         y->color = p->color;
//     }

//     if (y_original_color == RBTREE_BLACK) {
//          if (x == t->nil) {
//             x->parent = x_parent; // "사전 준비": nil 노드의 parent를 임시 설정
//         }

//         rb_delete_fixup(t, x);

//         // [핵심 확인 4] fixup이 끝난 후, nil 노드에 했던 특별 처리를 원상 복구합니다.
//         if (x == t->nil) {
//             x->parent = t->nil;   // "뒷정리": nil 노드의 parent를 원래대로 복원
//         }
//     }
//     free(p);
//     p=NULL;


//     return 0;
// }


// void rb_transplant(rbtree *t, node_t *u, node_t *v){ //u를 v로 교체
//   if(u->parent == t->nil){ //u가 root 면
//     t->root = v; //v로 root 변경
//   }
//   else if(u == u->parent->left){ //u가 왼쪽 자식이면
//     u->parent->left = v; //왼쪽에 v의 서브트리를 u를 가진 서브트리를 빼고 u의 부모 노드에 붙임
//   }
//   else{ //u가 오른쪽 자식이면
//     u->parent->right = v;//오른쪽에 v의 서브트리를 u를 가진 서브트리를 빼고 u의 부모 노드에 붙임
//   }
//   if (v != t->nil)
//         v->parent = u->parent;


// }

//https://gemini.google.com/app/e15e151713bb9781?is_sa=1&is_sa=1&android-min-version=301356232&ios-min-version=322.0&campaign_id=bkws&utm_source=sem&utm_source=google&utm_medium=paid-media&utm_medium=cpc&utm_campaign=bkws&utm_campaign=2024koKR_gemfeb&pt=9008&mt=8&ct=p-growth-sem-bkws&gclsrc=aw.ds&gad_source=1&gad_campaignid=20437330476&gbraid=0AAAAApk5BhmgwHAmtm4l5o9HivVtaCZtr&gclid=CjwKCAjwtfvEBhAmEiwA-DsKjlwKunUUtRxiepJiQq6Oocgr7L8JiZIgKvGV6SiUZ55sR_vCL4aipRoCiO8QAvD_BwE


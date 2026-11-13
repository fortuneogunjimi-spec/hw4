#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

int height(Node * root);

int height(Node * root) {
  if(root->left == NULL && root->right == NULL) {
      return 0;
    }
  if(root->left != NULL)
  {
    return 1 + height(root->left);
  }
  else {
    return 1 + height(root->right);
  }
}

bool equalPaths(Node * root)
{
    // Add your code below
    if(root == NULL) {return true;}

    if(root->left == NULL && root->right == NULL) {
      return true;
    }
    else if(root->left == NULL) {
      return equalPaths(root->right);
    }
    else if(root->right == NULL) {
      return equalPaths(root->left);
    }
    else {
      return (equalPaths(root->left) && equalPaths(root->right) && (height(root->left) == height(root->right)));
      
    }

}
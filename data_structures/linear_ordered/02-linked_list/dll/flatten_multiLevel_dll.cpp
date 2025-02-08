// Node *flatten(Node *head) {
//   Node *curr = head;
//   while (curr) {
//     if (curr->child) {
//       Node *next = curr->next;
//       curr->next = curr->child;
//       curr->next->prev = curr;
//       curr->child = nullptr;

//       Node *temp = curr->next;
//       while (temp->next)
//         temp = temp->next;
//       temp->next = next;
//       if (next)
//         next->prev = temp;
//     }
//     curr = curr->next;
//   }
//   return head;
// }

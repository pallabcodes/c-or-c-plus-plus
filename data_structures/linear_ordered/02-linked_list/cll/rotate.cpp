// Node *rotateCLL(Node *head, int k) {
//   if (!head || k == 0)
//     return head;

//   Node *tail = head;
//   int len = 1;
//   while (tail->next != head) {
//     tail = tail->next;
//     len++;
//   }

//   k = k % len;
//   if (k == 0)
//     return head;

//   Node *newTail = head;
//   for (int i = 0; i < len - k - 1; i++)
//     newTail = newTail->next;

//   Node *newHead = newTail->next;
//   newTail->next = head;
//   tail->next = newHead;

//   return newHead;
// }

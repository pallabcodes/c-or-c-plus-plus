// ListNode *mergeKLists(vector<ListNode *> &lists) {
//   auto cmp = [](ListNode *a, ListNode *b) { return a->val > b->val; };
//   priority_queue<ListNode *, vector<ListNode *>, decltype(cmp)> pq(cmp);

//   for (auto list : lists)
//     if (list)
//       pq.push(list);

//   ListNode dummy(0);
//   ListNode *tail = &dummy;

//   while (!pq.empty()) {
//     ListNode *node = pq.top();
//     pq.pop();
//     tail->next = node;
//     tail = node;
//     if (node->next)
//       pq.push(node->next);
//   }

//   return dummy.next;
// }

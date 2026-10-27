// list.c - singly linked list implementation (positions are 1-based)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

static node_t *node_alloc(elem value) {
  node_t *n = malloc(sizeof(node_t));
  if (n == NULL) { perror("malloc"); exit(1); }
  n->value = value;
  n->next = NULL;
  return n;
}

list_t *list_alloc() {
  list_t *l = malloc(sizeof(list_t));
  if (l == NULL) { perror("malloc"); exit(1); }
  l->head = NULL;
  return l;
}

void list_free(list_t *l) {
  if (l == NULL) return;
  node_t *cur = l->head;
  while (cur != NULL) {
    node_t *next = cur->next;
    free(cur);
    cur = next;
  }
  free(l);
}

char *listToString(list_t *l) {
  size_t cap = 64, len = 0;
  char *buf = malloc(cap);
  if (buf == NULL) { perror("malloc"); exit(1); }
  buf[0] = '\0';
  for (node_t *cur = l->head; cur != NULL; cur = cur->next) {
    char tmp[32];
    int n = snprintf(tmp, sizeof(tmp), "%d->", cur->value);
    if (len + n + 5 > cap) {
      while (len + n + 5 > cap) cap *= 2;
      buf = realloc(buf, cap);
      if (buf == NULL) { perror("realloc"); exit(1); }
    }
    memcpy(buf + len, tmp, n + 1);
    len += n;
  }
  strcpy(buf + len, "NULL");
  return buf;
}

void list_print(list_t *l) {
  char *s = listToString(l);
  printf("%s\n", s);
  free(s);
}

int list_length(list_t *l) {
  int count = 0;
  for (node_t *cur = l->head; cur != NULL; cur = cur->next) count++;
  return count;
}

void list_add_to_back(list_t *l, elem value) {
  node_t *n = node_alloc(value);
  if (l->head == NULL) { l->head = n; return; }
  node_t *cur = l->head;
  while (cur->next != NULL) cur = cur->next;
  cur->next = n;
}

void list_add_to_front(list_t *l, elem value) {
  node_t *n = node_alloc(value);
  n->next = l->head;
  l->head = n;
}

// Inserts value so it becomes the element at position `index` (1-based).
// Valid positions are 1..length; anything else is ignored.
void list_add_at_index(list_t *l, elem value, int index) {
  int len = list_length(l);
  if (index < 1 || index > len) return;
  if (index == 1) { list_add_to_front(l, value); return; }
  node_t *prev = l->head;
  for (int i = 1; i < index - 1; i++) prev = prev->next;
  node_t *n = node_alloc(value);
  n->next = prev->next;
  prev->next = n;
}

elem list_remove_from_back(list_t *l) {
  if (l->head == NULL) return -1;
  if (l->head->next == NULL) {
    elem v = l->head->value;
    free(l->head);
    l->head = NULL;
    return v;
  }
  node_t *prev = l->head;
  while (prev->next->next != NULL) prev = prev->next;
  elem v = prev->next->value;
  free(prev->next);
  prev->next = NULL;
  return v;
}

elem list_remove_from_front(list_t *l) {
  if (l->head == NULL) return -1;
  node_t *old = l->head;
  elem v = old->value;
  l->head = old->next;
  free(old);
  return v;
}

elem list_remove_at_index(list_t *l, int index) {
  int len = list_length(l);
  if (index < 1 || index > len) return -1;
  if (index == 1) return list_remove_from_front(l);
  node_t *prev = l->head;
  for (int i = 1; i < index - 1; i++) prev = prev->next;
  node_t *target = prev->next;
  elem v = target->value;
  prev->next = target->next;
  free(target);
  return v;
}

bool list_is_in(list_t *l, elem value) {
  for (node_t *cur = l->head; cur != NULL; cur = cur->next)
    if (cur->value == value) return true;
  return false;
}

elem list_get_elem_at(list_t *l, int index) {
  if (index < 1) return -1;
  int i = 1;
  for (node_t *cur = l->head; cur != NULL; cur = cur->next, i++)
    if (i == index) return cur->value;
  return -1;
}

int list_get_index_of(list_t *l, elem value) {
  int i = 1;
  for (node_t *cur = l->head; cur != NULL; cur = cur->next, i++)
    if (cur->value == value) return i;
  return -1;
}

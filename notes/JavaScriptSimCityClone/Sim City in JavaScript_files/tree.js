function Tree() {
  this.root = null;
}

Tree.prototype.hasNodes = function Tree_hasNodes() {
  return this.root ? true : false;
}

Tree.prototype.removeFirst = function Tree_removeFirst() {
  var node = this.root;
  if (!node) {
    throw "No node!";
  }
  if (!node.left) { // we're the first
    this.root = node.right; // bring the right child to the top
    return node;
  }

  // find the last node that has a left child
  var prevNode = node;
  while (node.left) {
    prevNode = node;
    node = node.left;
  }
  // Ok, now prevNode is the last guy that has a left child.
  // Return that child, and set prevNode.left to that child's right
  var toReturn = node;
  prevNode.left = node.right;
  return toReturn;
};

Tree.prototype.insert = function Tree_insert(newNode) {
  var node = this.root;
  if (!node) {
    this.root = newNode;
    return;
  }
	     
  var prevNode = node;
  while (node) {
    if (newNode.cost <= node.cost) {
      prevNode = node;
      node = node.left;
    }
    else {
      prevNode = node;
      node = node.right;
    }
  }
  // ok, prevNode is the node that should get newNode as a child
  if (newNode.cost <= prevNode.cost)
    prevNode.left = newNode;
  else
    prevNode.right = newNode;
};

function TreeNode(item, cost) {
  this.item = item;
  this.cost = cost;
  this.left = null;
  this.right = null;
}

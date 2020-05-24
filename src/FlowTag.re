/* TODO: doesn't currently support external functiosn properly */

type uid = string;

type flowNodeType =
  | Dummy /* used to resolve flows for child/grandchild/great-grandchild/etc. leaf nodes, but doesn't create transitions */
  | Leaf;

type flow = {
  flowNodeType,
  uid,
  rootPath: list(int),
};

type t = option(flow);

/* type t = {
     uid,
     rootPath: list(uid),
   };

   type flowMapInfo =
     | UID(uid)
     | Node(RenderLinks.node);

   module MS = Belt.Map.String;

   type flowMap = MS.t(flowMapInfo); */
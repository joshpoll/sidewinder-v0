open SidewinderUtil;

/* let box = child =>
     nest([child], ([c]) =>
       <div style={ReactDOMRe.Style.make(~display="inline-block", ~borderStyle="solid", ())}>
         {c.custom##rendered}
       </div>
     );

   let a = atom(React.string("2"));
   let a' = box(a);
   let b = atom(React.string({js|•|js}));
   let b' = box(b);
   let c = atom(React.string("4"));
   let c' = box(c);
   let d = atom(React.string("/"));
   let d' = box(d);
   let e = sequence([a', b'], _ => <> </>, LeftRight);
   let f = sequence([c', d'], _ => <> </>, LeftRight);
   /* TODO: need a nonlocal edge from b to f */
   let g = sequence([e, f], _ => <> </>, LeftRight); */
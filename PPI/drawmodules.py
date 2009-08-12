#!/usr/bin/python

import sys

mode = "MODULE"

sys.stdout.write("""
digraph Modules {
node [shape=box, fontsize=10, fontname="Helvetica", height=.2];
edge [labelfontsize=8, labelfontname="Helvetica"]
""")

modules = {}
connectors = {}

for line in sys.stdin:
    line = line.strip()
    if line == "":
        mode = "MODULE"
    elif mode == "MODULE":
        moduleid, module = line.split(' ',1);
        connectorid = type = peerid = None
        modules[moduleid] = (module, []);
        mode = "CONNECTOR"
    else:
        connectorid, type = line.split(' ',1);
        elts = type.rsplit(' ',1);
        if elts[-1].startswith('0x'):
            type, peerid = elts
        else:
            peerid = None
        modules[moduleid][1].append((connectorid, type, peerid))
        connectors[connectorid] = moduleid

for moduleid, (type, cs) in modules.iteritems():
    type = type.split('<',1)[0]
    type = type.rsplit('::',1)[-1]
    sys.stdout.write('"%s" [label="%s (%s)"]\n' % (moduleid,  type, moduleid))

anonid = 0

for moduleid, (type, cs) in modules.iteritems():
    for connectorid, type, peerid in cs:
        opts = []
        if "Passive" in type:
            opts.append("arrowtail=odot");
        if peerid:
            opts.append('headlabel="%s"' % peerid)
        opts.append('taillabel="%s"' % connectorid)
        opts = ",".join(opts)
        if opts:
            opts = " [%s]" % opts
        if "Output" in type and peerid is not None:
            sys.stdout.write('"%s" -> "%s"%s\n' % (moduleid, connectors[peerid],opts))
        elif peerid is None:
            sys.stdout.write('anon%d [label="", shape=point, height=.05];\n' % anonid)
            if "Output" in type:
                sys.stdout.write('"%s" -> anon%d%s;\n' % (moduleid, anonid, opts))
            else:
                sys.stdout.write('anon%d -> "%s"%s;\n' % (anonid, moduleid, opts))
            anonid += 1

sys.stdout.write("}\n")

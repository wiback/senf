#!/usr/bin/python

import sys

mode = "MODULE"

sys.stdout.write("""
digraph Modules {
node [shape=Mrecord, fontsize=8, fontname="Helvetica"];
""")

modules = {}
connectors = {}

for line in sys.stdin:
    line = line.strip()
    if line == "":
        mode = "MODULE"
    elif mode == "MODULE":
        moduleid, module = line.split(' ',1);
        moduleid = moduleid[1:]
        connectorid = type = peerid = None
        modules[moduleid] = (module, []);
        mode = "CONNECTOR"
    else:
        connectorid, type = line.split(' ',1);
        connectorid = connectorid[1:]
        elts = type.rsplit(' ',1);
        if elts[-1].startswith('0x'):
            type, peerid = elts
            peerid = peerid[1:]
        else:
            peerid = None
        modules[moduleid][1].append((connectorid, type, peerid))
        connectors[connectorid] = moduleid

for moduleid, (module, cs) in modules.iteritems():
    module = module.split('<',1)[0]
    module = module.rsplit('::',1)[-1]
    inputs = []
    outputs = []
    for connectorid, type, peerid in cs:
        if 'Input' in type: inputs.append("<%s>%s" % (connectorid,connectorid))
        else:               outputs.append("<%s>%s" % (connectorid,connectorid))
    rows = []
    if inputs: rows.append("{%s}" % "|".join(inputs))
    rows.append("%s (%s)" % (module, moduleid))
    if outputs: rows.append("{%s}" % "|".join(outputs))
    sys.stdout.write('%s [label="{%s}"]\n' % (moduleid, "|".join(rows) ))

anonid = 0

for moduleid, (type, cs) in modules.iteritems():
    for connectorid, type, peerid in cs:
        opts = []
        if "Passive" in type: opts.append("arrowtail=odot");
        opts = ",".join(opts)
        if opts: opts = " [%s]" % opts
        if "Output" in type and peerid is not None:
            sys.stdout.write('%s:%s -> %s:%s%s\n' 
                             % (moduleid, connectorid, connectors[peerid], peerid, opts))
        elif peerid is None:
            sys.stdout.write('anon%d [label="", shape=point, height=.05];\n' % anonid)
            if "Output" in type:
                sys.stdout.write('%s:%s -> anon%d%s;\n' % (moduleid, connectorid, anonid, opts))
            else:
                sys.stdout.write('anon%d -> %s:%s%s;\n' % (anonid, moduleid, connectorid, opts))
            anonid += 1

sys.stdout.write("}\n")

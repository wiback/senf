#!/usr/bin/python

import sys

COLOR_SCHEME = 'pastel19'  # see http://www.graphviz.org/doc/info/colors.html
NETEMU = ["InterfaceAnnotater" , "PacketMonitor" , "PacketTypeChecker"]
mode = "MODULE"

sys.stdout.write("""
digraph Modules {
node [shape=Mrecord, fontsize=8, fontname="Helvetica"];
""")

modules = {}
connectors = {}
p = 0

for opt in sys.argv:
   if opt in ("-p","--packet"):
        p = 1

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
	
	packet = type.rsplit('<',1)[1];
	packet = packet.rsplit('::',1)[-1];
	packet = packet.split('>',1)[0];
		
	elts = type.rsplit(' ',1);
        if elts[-1].startswith('0x'):
            type, peerid = elts
            peerid = peerid[1:]
        else:
            peerid = None
        modules[moduleid][1].append((connectorid, type, peerid, packet))
        connectors[connectorid] = moduleid

for moduleid, (module, cs) in modules.iteritems():
    module = module.split('<',1)[0]
    if module.rsplit('::',1)[-1] in NETEMU or module.startswith("senf::emu"):
        color = 5
    elif module.endswith("Source") or module.endswith("Sink"):
        color = 1
    elif "senf" not in module.split('::',1)[0]:
        color = 6
    else:
        color = 3
    module = module.rsplit('::',1)[-1]
    inputs = []
    outputs = []
    for connectorid, type, peerid,packet in cs:
        if 'Input' in type: inputs.append("<%s>%s" % (connectorid,connectorid))
        else:               outputs.append("<%s>%s" % (connectorid,connectorid))
    rows = []
    if inputs: rows.append("{%s}" % "|".join(inputs))
    rows.append("%s (%s)" % (module, moduleid))
    if outputs: rows.append("{%s}" % "|".join(outputs))
    sys.stdout.write('%s [label="{%s}" style="filled" fillcolor="/%s/%s"  ]\n'
                             % (moduleid, "|".join(rows),COLOR_SCHEME, color ))

anonid = 0

for moduleid, (type, cs) in modules.iteritems():
    for connectorid, type, peerid, packet in cs:
        opts = []
        if "Passive" in type: opts.append("arrowtail=odot");
        opts = ",".join(opts)
        if opts: opts = " [%s]" % opts
        if "Output" in type and peerid is not None:
	    if "Packet" in packet and p is 1:
            	sys.stdout.write('%s:%s -> %s:%s%s [label=" %s", fontsize=8, fontname="Helvetica"]\n' 
                             % (moduleid, connectorid, connectors[peerid], peerid, opts,packet))
	    else:
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


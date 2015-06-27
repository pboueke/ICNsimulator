#--------------------------------------------#
#             Topology Generator             #
#--------------------------------------------#

class Node:
	#Class for storing Node information
	def connect_node_above(self, n):
		self.n_node_above = n
	def connect_node_domain(self, n):
		self.n_nodes_domain.append(n)
	def __init__(self, i, n, t, d, c):
		self.n_id = i
		self.n_name = n
		self.n_type = t
		self.n_domain = d
		self.n_cache = c
		self.n_node_above = 0
		self.n_nodes_domain = []
	def __str__(self):
		s = "[NodeParams]\n"
		s += "Type=%d\n" % (self.n_type)
		s += "Name=%s\n" % (self.n_name)
		s += "Ident=%d\n" % (self.n_id)
		s += "DomainIdent=%d\n" % (self.n_domain)
		s += "CacheSize=%d;%d\n" % (self.n_cache[0], self.n_cache[1])
		s += "NodesAboveConnected="
		s += "%d\n" % (self.n_node_above)
		s += "DomainnodesConnected="
		for node in self.n_nodes_domain:
			s += "%d;" % (node)
		if (len(self.n_nodes_domain) > 0): s = s[:-1]
		s += "\n"
		return s

class Domain:
	#Class for storing Domain information
	def __init__(self, i, t, p):
		self.d_id = i
		self.d_type = t
		self.d_param = p
	def __str__(self):
		s = "[TransmissionDistParams]\n"
		s += "Domain=%d\n" % (self.d_id)
		s += "Type=%d\n" % (self.d_type)
		s += "Params=%d\n" % (self.d_param)
		return s

class Content:
	#Class for storing Content information
	def __init__(self, n, s):
		self.c_name = n
		self.c_size = s
	def __str__(self):
		s = "[ContentParams]\n"
		s += "Name=%s\n" % (self.c_name)
		s += "Size=%d\n" % (self.c_size)
		return s

#--------------------------------------------#

def write_header():
	SimTime = 100000
	TTL = int(raw_input("Enter TTL Limit: "))
	CacheAlgorithm = 0
	DebugOper = 1
	DebugRouter = 0
	DebugClient = 0

	IsEnabled = 0
	MinTime = 0
	MaxTime = 100

	Upper = 5
	Lower = 5

	Type = 0
	Params = 25

	s = "SimTime=%d\n" % (SimTime)
	s += "TTLLimits=%d\n" % (TTL)
	s += "CacheAlgorithm=%d\n" % (CacheAlgorithm)
	s += "DebugOper=%d\n" % (DebugOper)
	s += "DebugRouter=%d\n" % (DebugRouter)
	s += "DebugClient=%d\n\n" % (DebugClient)
	s += "[Trace]\nIsEnabled=%d\n" % (IsEnabled)
	s += "MinTime=%d\n" % (MinTime)
	s += "MaxTime=%d\n\n" % (MaxTime)
	s += "[RCLimits]\nUpper=%d\n" % (Upper)
	s += "Lower=%d\n\n" % (Lower)
	s += "[RequestDistParams]\nType=%d\n" % (Type)
	s += "Params=%d\n" % (Params)

	return s

def gen_domains():
	n_domains = int(raw_input("Enter number of domains: "))
	domains = []
	for i in range(n_domains):
		typ = 0
		param = 26
		domains.append(Domain(i, typ, param))
	return domains

def write_sub_header():
	Type = 2
	Params = (1,0)

	s = "[RandomDistParams]\nType=%d\n" % (Type)
	s += "Params=%d;%d\n\n" % (Params[0],Params[1])

	Type = 3
	Params = 1.2

	s += "[ContentDistParams]\nType=%d\n" % (Type)
	s += "Params="+str(Params)+"\n\n"

	Type = 0
	Params = 14.28

	s += "[RCDistParams]\nType=%d\n" % (Type)
	s += "Params="+str(Params)+"\n"

	return s

def gen_nodes(d):
	n_nodes = int(raw_input("Enter number of nodes: "))
	n_clients = int(raw_input("Enter number of clients: "))
	domains = []
	for i in range(d): domains.append([])
	nodes = []
	for i in range(1,n_clients+1):
		node = Node(i, "client"+str(i), 0, 0, (3,0))
		node.connect_node_above(int(raw_input("Enter router id for client %d: " % (i))))
		nodes.append(node)
	for i in range(1,n_nodes-n_clients):
		dom = int(raw_input("Enter router domain id for router %d: " % (i)))
		node = Node(i+n_clients, "router"+str(i), 1, dom, (3,0))
		node.connect_node_above(int(raw_input("Enter above id for router %d: " % (i))))
		nodes.append(node)
		domains[dom].append(node)
	for domain in domains:
		for node in domain:
			for other in domain:
				if (node==other): continue
				node.connect_node_domain(other.n_id)
	nodes.append(Node(n_nodes, "nrs1", 2, 0, (3,0)))
	return nodes

def gen_contents():
	n_contents = int(raw_input("Enter number of contents: "))
	contents = []
	for i in range(1, n_contents+1):
		contents.append(Content("content"+str(i), 1))
	return contents

#--------------------------------------------#

header = write_header()
domains = gen_domains()
sub_header = write_sub_header()
nodes = gen_nodes(len(domains))
contents = gen_contents()

out = open("topology.txt", 'w')
out.write(header+"\n")
for domain in domains:
	out.write(str(domain)+"\n")
out.write(sub_header+"\n")
out.write("[Topology]\n")
out.write("NodesNumber=%d\n" % (len(nodes)))
out.write("DomainsNumber=%d\n\n" % (len(domains)))
for node in nodes:
	out.write(str(node)+"\n")
out.write("[ContentItems]\n")
out.write("ContentsNumber=%d\n\n" % (len(contents)))
for content in contents:
	out.write(str(content)+"\n")
out.close()

print "END"


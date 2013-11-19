#This script takes in several values from the user 
#It then creates a randomizes graph with N nodes
#The number of nodes and resulting edges are written to the file
#The first line is the number of nodes in the graph
#The remaining lines of the file contain the edges
#Created by: Gary Patricelli 11/14/2013

import random

#Get number of nodes
valid_answer = False
while not valid_answer:
    print 'How many nodes should there be?'
    N = int(raw_input('> '))
    #Check input
    if N > 0:
        valid_answer = True
    else:
        print str(N) + ' is not a valid input'
        print


#Get the graph desity
valid_answer = False
while not valid_answer:
    print 'How dense should the graph be?'
    print 'Enter an integer between 1 and 5 inclusive'
    density = float(raw_input('> '))
    #Check input
    if density >= 1 and density <=5 and density % 1 == 0:
        #This is valid
        density = int(density)
        valid_answer = True
    else:
        print str(density) + ' is not a valid input'
        print

#Get the name of the output file
print 'What should the file be called?'
print '.txt will be automaticly appended'
file_name = str(raw_input('> '))

#Calculate number of edges
number_of_edges = (N * density)

#Create the random edges
edges = []
edge_count = number_of_edges

while edge_count != 0:
    #Pick a random source
    source = random.randint(0, N-1)
    #Pick a random sink
    sink = random.randint(0, N-1)
    
    edge = str(source) + ' ' + str(sink)
    #Check if this edge is in the list of edges
    #Check if this is a self looping edge
    if not (edge in edges) and (source != sink):
        #Add this to the list of edges 
        edges.append(edge)
        edge_count = edge_count - 1
    
#Create the file
f = open(str(file_name) + '.txt', 'w')
#Write the number of nodes
f.write(str(N) + '\n')
#Write the number of edges 
f.write(str(number_of_edges) + '\n')
for edge in edges:
    f.write(edge + '\n')
f.close()

#Let the user know the program is done
print file_name  + '.txt has be created'
print 'Exiting program.'

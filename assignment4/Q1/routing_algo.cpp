#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
	for (int i = 0; i < nd.size(); i++) {
	  nd[i]->printTable();
	}
}

void routingAlgo(vector<RoutingNode*> nd){

    /*Print routing table entries before routing algo converges */
    cout<<"routing table entries before routing algo converges\n";
    printRT(nd);

    int no_of_iterations = 3;
    while( no_of_iterations-- )
    {
        for(int i = 0; i < nd.size(); i++ )
        {
            nd[i]->sendMsg();
        }
    }

    cout<<"\n--------**************************************---------\n";

    /*Print routing table entries after routing algo converges */
    cout<<"routing table entries after routing algo converges\n";
    printRT(nd);

}


void RoutingNode::recvMsg(RouteMsg *msg) {

  //your code here
  //starting loop on routing table of sender
  for(int i = 0;i < msg->mytbl->tbl.size(); i++)
  {
      int check = 0, ind;

      //finding entries of sender's in receiver's table
      for(int j = 0; j < mytbl.tbl.size(); j++)
      {

        // if entry found in routing table of reciever then break loop and save its index
        if(mytbl.tbl[j].dstip == msg->mytbl->tbl[i].dstip)
        {
            ind = j;
            check = 1;
            break;
        }

      }

      // if entry not found then create a new entry for that destip and insert in reciever's table
      if(check == 0)
      {

          //creating new routing entry
          RoutingEntry new_entry;
          new_entry.dstip = msg->mytbl->tbl[i].dstip;
          new_entry.ip_interface = msg->recvip;
          new_entry.nexthop = msg->from;
          
          //if new entry is not for direct link then update cost from by senders cost+link cost to reach sender.
          if(msg->from != new_entry.dstip)
          {
            new_entry.cost = min(16, msg->mytbl->tbl[i].cost + 1);
          }
          else
          {
            //else link cost of sender
            new_entry.cost = 1;
          }
            
          //insert in table
          mytbl.tbl.push_back(new_entry);

      }
      //else if entry of sender's table s found in reciever's table update it accordingly
      else
      {
          //if entry is interface of reciever then its cost would not change as it will always be 0.
          if(this->isMyInterface(mytbl.tbl[ind].dstip))
          {

          }
          //else update it by min of this iteration.
          else if(mytbl.tbl[ind].cost > msg->mytbl->tbl[i].cost+1)
          {
              mytbl.tbl[ind].cost = min(16, msg->mytbl->tbl[i].cost+1);
              mytbl.tbl[ind].nexthop = msg->from;
          }
      }

    }
}
//ref-a little help from : github/krishnaw14




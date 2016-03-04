// Copyright (c) 2016 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#ifndef REQUEST_MANAGER_H
#define REQUEST_MANAGER_H
#include "net.h"
#include "stat.h"

class CUnknownObj
{
public:
  enum
    {
      MAX_AVAIL_FROM = 8  // We don't really care if the txn is available from more than 8 nodes... that should be enough to grab it.
    };
  CInv obj;
  bool rateLimited;
  int64_t lastRequestTime;  // In microseconds, 0 means no request
  unsigned int outstandingReqs;
  unsigned int receivingFrom;
  CNode* availableFrom[MAX_AVAIL_FROM];

  
  CUnknownObj()
  {
    for (int i =0;i<MAX_AVAIL_FROM;i++) availableFrom[i] = NULL;
    rateLimited = false;
    receivingFrom = 0;
    outstandingReqs = 0;
    lastRequestTime = 0;
  }

  //CUnknownObj( const CUnknownObj &) = default;
};

class CRequestManager
{
  protected:
  // map of transactions
  typedef std::map<uint256, CUnknownObj> OdMap;
  OdMap mapTxnInfo;
  CCriticalSection cs_objDownloader;

  OdMap::iterator sendIter;

  int inFlight;
  int maxInFlight;
  CStatHistory<int> inFlightTxns;
  CStatHistory<int> receivedTxns;
  CStatHistory<int> rejectedTxns;
  CStatHistory<int> droppedTxns;
  
  void cleanup(OdMap::iterator& item);

  public:
  CRequestManager();

  // Get this object from somewhere, asynchronously.
  void AskFor(const CInv& obj, CNode* from);

  // Get these objects from somewhere, asynchronously.
  void AskFor(const std::vector<CInv>& objArray, CNode* from);

  // Indicate that we got this object, from and bytes are optional (for node performance tracking)
  void Received(const CInv& obj, CNode* from=NULL, int bytes=0);

  // Indicate that getting this object was rejected
  void Rejected(const CInv& obj, CNode* from=NULL, unsigned char reason=0);
  
  void SendRequests();

};


extern CRequestManager requester;  // Singleton class

#endif

/*
 * ====================================================================
 * The Vovida Software License, Version 1.0
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 * and "Vovida Open Communication Application Library (VOCAL)" must
 * not be used to endorse or promote products derived from this
 * software without prior written permission. For written
 * permission, please contact vocal@vovida.org.
 * 
 * 4. Products derived from this software may not be called "VOCAL", nor
 * may "VOCAL" appear in their name, without prior written
 * permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 * 
 */

package vocal.data;

import vocal.comm.VPPTransactionWrapper;
import vocal.comm.VPPException;

import java.io.File;
import java.io.StringReader;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.net.ConnectException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.w3c.dom.Element;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * When a new system is deployed, there is a process that automatically
 * creates servers and assigns them to the hosts that make up the system.
 * This process stores its Provisioning information in a file called
 * DeployedServers.xml. The Deploy process then runs the main method of
 * this class to populate the Provisioning data with the deployed servers.
 */
public class DeployServers extends DefaultHandler implements TreeNodeTypes, 
        DataDefaults
{

  // the connection to pserver
  private static VPPTransactionWrapper connection = null;

  // the DOM tree we are dealing with.
  private Document baseDocument;

  // nodes in System
  private Element globalConfigElement;
  private Element ospServerElement;
  private Element ipPlanElement;
  private Element phonePlanElement;

  // the feature nodes we will be dealing with
  private Element forwardAllCallsGroupElement;
  private Element forwardNoAnswerBusyGroupElement;
  private Element callBlockingGroupElement;
  private Element callScreeningGroupElement;
  private Element voicemailGroupElement;
  private Element callReturnGroupElement;
  private Element callerIdBlockingGroupElement;

  // the marshal nodes we will be dealing with
  private Element userAgentGroupElement;
  private Element gatewayGroupElement;
  private Element conferenceBridgeGroupElement;
  private Element internetworkGroupElement;

  // the other server nodes we will be dealing with
  private Element redirectGroupElement;
  private Element cdrGroupElement;
  private Element pdpGroupElement;
  private Element heartbeatGroupElement;

  /**
   */
  public DeployServers()
  {
    try
    {
      StringReader baseReader = new StringReader(BASE_XML);

      DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
      DocumentBuilder parser = null;
      parser = dbf.newDocumentBuilder();

      baseDocument = parser.parse(new InputSource(baseReader));

      Element rootElement = baseDocument.getDocumentElement();
      Element systemElement = XMLUtils.getChildByName(rootElement, "system");
      Element serversElement = XMLUtils.getChildByName(rootElement, 
              "servers");

      // global config
      StringReader globalConfigReader = new StringReader(GLOBAL_CONFIG_XML);

      globalConfigElement = 
        parser.parse(new InputSource(globalConfigReader)).getDocumentElement();

      systemElement.appendChild(baseDocument.importNode(globalConfigElement, 
              true));
      // set globalConfigElement to the real one so we can use it later.
      try
      {
        globalConfigElement
          = XMLUtils.getChildByName(systemElement, "globalConfiguration");
      }
      catch (Exception e)
      {
        e.printStackTrace();

        return;
      }
      
      // OSP
      StringReader ospReader = new StringReader(OSP_SERVER_XML);

      ospServerElement
        = parser.parse(new InputSource(ospReader)).getDocumentElement();;


      systemElement.appendChild(baseDocument.importNode(ospServerElement, 
              true));

      // IP Dial Plan
      StringReader phonePlanReader = new StringReader(DEFAULT_IP_PLAN_XML);

      ipPlanElement = 
        parser.parse(new InputSource(phonePlanReader)).getDocumentElement();
      


      systemElement.appendChild(baseDocument.importNode(ipPlanElement, true));

      // Default Dial Plan
      StringReader ipPlanReader  = new StringReader(DEFAULT_PHONE_PLAN_XML);

      phonePlanElement = 
        parser.parse(new InputSource(ipPlanReader)).getDocumentElement();
      


      systemElement.appendChild(baseDocument.importNode(phonePlanElement, 
              true));

      // Feature Servers
      StringReader listOfFeatureReader =
        new StringReader(LIST_OF_FEATURE_SERVERS_XML);

      Element featureServersElement
        = parser.parse(new InputSource(listOfFeatureReader)).getDocumentElement();

      serversElement.appendChild(baseDocument.importNode(featureServersElement, 
              true));

      // Marshal Servers
      StringReader listOfMarshalReader = new StringReader(LIST_OF_MARSHAL_SERVERS_XML);

      Element marshalServersElement = 
      parser.parse(new InputSource(listOfMarshalReader)).getDocumentElement();

      serversElement.appendChild(baseDocument.importNode(marshalServersElement, 
              true));

      // Redirect Servers
      StringReader listOfRedirectReader
        = new StringReader(LIST_OF_REDIRECT_SERVERS_XML);

      Element redirectServersElement = 
      parser.parse(new InputSource(listOfRedirectReader)).getDocumentElement();

      serversElement.appendChild(baseDocument.importNode(redirectServersElement, 
              true));

      // CDR Servers
      StringReader listOfCdrReader = new StringReader(LIST_OF_CDR_SERVERS_XML);

      Element cdrServersElement =
        parser.parse(new InputSource(listOfCdrReader)).getDocumentElement();


      serversElement.appendChild(baseDocument.importNode(cdrServersElement, 
              true));

      // PDP Servers
      StringReader listOfPdpReader = new StringReader(LIST_OF_PDP_SERVERS_XML);

      Element pdpServersElement = 
        parser.parse(new InputSource(listOfPdpReader)).getDocumentElement();
      


      serversElement.appendChild(baseDocument.importNode(pdpServersElement, 
              true));

      // Heartbeat Servers
      StringReader heartbeatReader = new StringReader(LIST_OF_HEARTBEAT_SERVERS_XML);

      Element heartbeatServersElement = 
      parser.parse(new InputSource(heartbeatReader)).getDocumentElement();

      serversElement.appendChild(baseDocument.importNode(heartbeatServersElement, 
              true));

      NodeList nodes;

      // get the nodes for main server categories
      nodes = baseDocument.getElementsByTagName("featureServers");

      featureServersElement = (Element) nodes.item(0);

      if (featureServersElement == null)
      {
        throw new NoSuchNodeException("featureServersElement");
      }

      nodes = baseDocument.getElementsByTagName("marshalServers");

      marshalServersElement = (Element) nodes.item(0);

      if (marshalServersElement == null)
      {
        throw new NoSuchNodeException("marshalServersElement");
      }


      nodes = baseDocument.getElementsByTagName("redirectServers");

      redirectServersElement = (Element) nodes.item(0);

      if (redirectServersElement == null)
      {
        throw new NoSuchNodeException("redirectServersElement");
      }


      nodes = baseDocument.getElementsByTagName("cdrServers");

      cdrServersElement = (Element) nodes.item(0);

      if (cdrServersElement == null)
      {
        throw new NoSuchNodeException("cdrServersElement");
      }


      nodes = baseDocument.getElementsByTagName("pdpServers");

      pdpServersElement = (Element) nodes.item(0);

      if (pdpServersElement == null)
      {
        throw new NoSuchNodeException("cdrServersElement");
      }

      nodes = baseDocument.getElementsByTagName("heartbeatServers");

      heartbeatServersElement = (Element) nodes.item(0);

      if (heartbeatServersElement == null)
      {
        throw new NoSuchNodeException("heartbeatServersElement");
      }


      // set up the feature server groups
      nodes = featureServersElement.getElementsByTagName("serverType");

      for (int i = 0; i < nodes.getLength(); i++)
      {
        Element featureTypeElement = (Element) nodes.item(i);
        NodeList featureTypeNodes = 
          featureTypeElement.getElementsByTagName("serverGroup");
        Element featureGroupElement = (Element) featureTypeNodes.item(0);

        if (featureGroupElement == null)
        {
          throw new NoSuchNodeException("featureGroupElement");
        }

        // find out which type of group we have
        String typeName = featureTypeElement.getAttribute("value");

        if (typeName.equals("ForwardAllCalls"))
        {
          forwardAllCallsGroupElement = featureGroupElement;

          continue;
        }

        if (typeName.equals("ForwardNoAnswerBusy"))
        {
          forwardNoAnswerBusyGroupElement = featureGroupElement;

          continue;
        }

        if (typeName.equals("CallBlocking"))
        {
          callBlockingGroupElement = featureGroupElement;

          continue;
        }

        if (typeName.equals("CallScreening"))
        {
          callScreeningGroupElement = featureGroupElement;

          continue;
        }

        if (typeName.equals("Voicemail"))
        {
          voicemailGroupElement = featureGroupElement;

          continue;
        }

        if (typeName.equals("CallReturn"))
        {
          callReturnGroupElement = featureGroupElement;

          continue;
        }

        if (typeName.equals("CallerIdBlocking"))
        {
          callerIdBlockingGroupElement = featureGroupElement;

          continue;
        }

        System.out.println("unrecognized typName: " + typeName);

      }

      // set up the marshal server groups
      nodes = marshalServersElement.getElementsByTagName("serverType");

      for (int i = 0; i < nodes.getLength(); i++)
      {
        Element marshalTypeElement = (Element) nodes.item(i);
        NodeList marshalTypeNodes = 
          marshalTypeElement.getElementsByTagName("serverGroup");
        Element marshalGroupElement = (Element) marshalTypeNodes.item(0);

        if (marshalGroupElement == null)
        {
          throw new NoSuchNodeException("marshalGroupElement");
        }

        // find out which type of group we have
        String typeName = marshalTypeElement.getAttribute("value");

        if (typeName.equals("UserAgent"))
        {
          userAgentGroupElement = marshalGroupElement;

          continue;
        }

        if (typeName.equals("Gateway"))
        {
          gatewayGroupElement = marshalGroupElement;

          continue;
        }

        if (typeName.equals("ConferenceBridge"))
        {
          conferenceBridgeGroupElement = marshalGroupElement;

          continue;
        }

        if (typeName.equals("Internetwork"))
        {
          internetworkGroupElement = marshalGroupElement;

          continue;
        }

        System.out.println("unrecognized type name: " + typeName);
      }

      // set up the other server group nodes
      nodes = redirectServersElement.getElementsByTagName("serverGroup");
      redirectGroupElement = (Element) nodes.item(0);

      if (redirectGroupElement == null)
      {
        throw new NoSuchNodeException("redirectGroupElement");
      }

      nodes = cdrServersElement.getElementsByTagName("serverGroup");
      cdrGroupElement = (Element) nodes.item(0);

      if (cdrGroupElement == null)
      {
        throw new NoSuchNodeException("cdrGroupElement");
      }

      nodes = pdpServersElement.getElementsByTagName("serverGroup");
      pdpGroupElement = (Element) nodes.item(0);

      if (pdpGroupElement == null)
      {
        throw new NoSuchNodeException("pdpGroupElement");
      }

      nodes = heartbeatServersElement.getElementsByTagName("serverGroup");
      heartbeatGroupElement = (Element) nodes.item(0);

      if (heartbeatGroupElement == null)
      {
        throw new NoSuchNodeException("heartbeatGroupElement");
      }

    }
    catch (Exception e)
    {
      e.printStackTrace();

      return;
    }

  }

  /**
   * 
   */
  public static VPPTransactionWrapper getConnection()
  {
    return connection;
  }

  /**
   */
  private void processDeployFile()
  {
    System.out.println("parsing deployment file");

    SAXParserFactory spf = SAXParserFactory.newInstance();
    SAXParser parser = null;
    try
    {
      parser = spf.newSAXParser();
    }
    catch (ParserConfigurationException parsEx)
    {
      parsEx.printStackTrace();
    }
    catch(SAXException saxEx)
    {
      saxEx.printStackTrace();
    }

    try
    {
      File xmlFile = new File(DEPLOYED_SERVERS_FILE);
      parser.parse(xmlFile, this);
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }


  /**
   *
   */
  private void handleGlobal(Attributes attrs)
    throws InvalidRequestException
  {
    String multicastHost = attrs.getValue("multicastHost");
    String multicastPort = attrs.getValue("multicastPort");
    Element multicastHostElement = null;
    try
    {
      multicastHostElement
       = XMLUtils.getChildByName(globalConfigElement, "multicastHost");
    }
    catch (Exception e)
    {
      e.printStackTrace();

      return;
    }
    if (multicastHost == null)
    {
      System.out.println("global lacking multicastHost attribute");

      return;
    }
    else
    {
      multicastHostElement.setAttribute("host", multicastHost);
    }
    if (multicastPort == null)
    {
      System.out.println("global lacking multicastPort attribute");

      return;
    }
    else
    {
      multicastHostElement.setAttribute("port", multicastPort);
    }
  }
 

  /**
   * @throws InvalidRequestException
   */
  private void handleFeatureServer(Attributes attrs) 
    throws InvalidRequestException
  {
    String featureType = attrs.getValue("type");

    if (featureType == null)
    {
      System.out.println("feature lacking type attribute");

      return;
    }

    Element groupElement = null;

    if (featureType.equals("ForwardNoAnswerBusy"))
    {
      groupElement = forwardNoAnswerBusyGroupElement;
    }
    else if (featureType.equals("ForwardAllCalls"))
    {
      groupElement = forwardAllCallsGroupElement;
    }
    else if (featureType.equals("CallBlocking"))
    {
      groupElement = callBlockingGroupElement;
    }
    else if (featureType.equals("CallScreening"))
    {
      groupElement = callScreeningGroupElement;
    }
    else if (featureType.equals("Voicemail"))
    {
      groupElement = voicemailGroupElement;

      FeatureData dataManager = new DataFragment(FEATURE_SERVER, 
              groupElement);

      dataManager.setHostData(attrs.getValue("host"));
      dataManager.setPortData(attrs.getValue("port"));
      dataManager.setUaVMHostData(attrs.getValue("uaVMHost"));
      dataManager.setUaVMFirstPortData(attrs.getValue("uaVMFirstPort"));
      dataManager.setUaVMLastPortData(attrs.getValue("uaVMLastPort"));
      dataManager.saveData();
      groupElement.appendChild(dataManager.getFragment());

      return;
    }
    else if (featureType.equals("CallReturn"))
    {
      groupElement = callReturnGroupElement;
    }
    else if (featureType.equals("CallerIdBlocking"))
    {
      groupElement = callerIdBlockingGroupElement;
    }
    else
    {
      System.out.println("Unrecognized type: " + featureType);

      return;
    }

    if (groupElement == null)
    {
      System.out.println("Error: group element is null");

      return;
    }

    FeatureData dataManager = new DataFragment(FEATURE_SERVER, groupElement);

    dataManager.setHostData(attrs.getValue("host"));
    dataManager.setPortData(attrs.getValue("port"));
    dataManager.saveData();
    groupElement.appendChild(dataManager.getFragment());
  }

  /**
   * 
   * @param attrs
   * 
   * @throws InvalidRequestException
   */
  private void handleMarshalServer(Attributes attrs) 
    throws InvalidRequestException
  {
    Element groupElement = null;
    MarshalData dataManager = null;
    String marshalType = attrs.getValue("type");

    if (marshalType == null)
    {
      System.out.println("marshalType is null");

      return;
    }

    if (marshalType.equals("UserAgent"))
    {
      groupElement = userAgentGroupElement;
      dataManager = new DataFragment(MARSHAL_SERVER, groupElement);

      dataManager.setHostData(attrs.getValue("host"));
      dataManager.setPortData(attrs.getValue("port"));
    }
    else if (marshalType.equals("Gateway"))
    {
      groupElement = gatewayGroupElement;
      dataManager = new DataFragment(MARSHAL_SERVER, groupElement);

      dataManager.setHostData(attrs.getValue("host"));
      dataManager.setPortData(attrs.getValue("port"));
      dataManager.setGatewayHostData(attrs.getValue("gatewayHost"));
      dataManager.setGatewayPortData(attrs.getValue("gatewayPort"));
    }
    else if (marshalType.equals("Internetwork"))
    {
      groupElement = internetworkGroupElement;
      dataManager = new DataFragment(MARSHAL_SERVER, groupElement);

      dataManager.setHostData(attrs.getValue("host"));
      dataManager.setPortData(attrs.getValue("port"));
      dataManager.setGatewayHostData(attrs.getValue("gatewayHost"));
      dataManager.setGatewayPortData(attrs.getValue("gatewayPort"));
    }
    else if (marshalType.equals("ConferenceBridge"))
    {
      groupElement = conferenceBridgeGroupElement;
      dataManager = new DataFragment(MARSHAL_SERVER, groupElement);

      dataManager.setHostData(attrs.getValue("host"));
      dataManager.setPortData(attrs.getValue("port"));
      dataManager.setGatewayHostData(attrs.getValue("gatewayHost"));
      dataManager.setGatewayPortData(attrs.getValue("gatewayPort"));
      dataManager.setConferenceBridgeNumberData(attrs.getValue("conferenceBridgeNumber"));
      dataManager.setConferenceBridgeNumberData(attrs.getValue("conferenceBridgeNumber"));
    }
    else
    {
      System.out.println("Unrecognized type: " + marshalType);

      return;
    }

    dataManager.saveData();
    groupElement.appendChild(dataManager.getFragment());
  }
  

  /**
   * 
   * @param attrs
   * 
   * @throws InvalidRequestException
   */
  private void handleRedirectServer(Attributes attrs) 
    throws InvalidRequestException
  {
    RedirectData dataManager = new DataFragment(REDIRECT_SERVER, 
            redirectGroupElement);

    dataManager.setHostData(attrs.getValue("host"));
    dataManager.setPortData(attrs.getValue("port"));
    dataManager.setSyncPortData(attrs.getValue("syncPort"));
    dataManager.saveData();
    redirectGroupElement.appendChild(dataManager.getFragment());
  }

  /**
   * 
   * @param attrs
   * 
   * @throws InvalidRequestException
   */
  private void handleCdrServer(Attributes attrs) 
    throws InvalidRequestException
  {
    CdrData dataManager = new DataFragment(CDR_SERVER, cdrGroupElement);

    dataManager.setHostData(attrs.getValue("host"));
    dataManager.setPortData(attrs.getValue("port"));
    dataManager.setRadiusHostData(attrs.getValue("radiusServerHost"));
    dataManager.setRadiusSecretKeyData(attrs.getValue("radiusSecretKey"));
    dataManager.setBillingDirPathData(attrs.getValue("billingDirPath"));

    String billForRingtime = attrs.getValue("billForRingtime");

    if (billForRingtime.equals("true"))
    {
      dataManager.setBillForRingtimeData(true);
    }
    else
    {
      dataManager.setBillForRingtimeData(false);
    }

    dataManager.saveData();
    cdrGroupElement.appendChild(dataManager.getFragment());
  }

  /**
   * 
   * @param attrs
   * 
   * @throws InvalidRequestException
   */
  private void handlePdpServer(Attributes attrs) 
    throws InvalidRequestException
  {
    PDPData dataManager = new DataFragment(PDP_SERVER, pdpGroupElement);

    dataManager.setHostData(attrs.getValue("host"));
    dataManager.saveData();
    pdpGroupElement.appendChild(dataManager.getFragment());
  }

  /**
   * 
   * @param attrs
   * 
   * @throws InvalidRequestException
   */
  private void handleHeartbeatServer(Attributes attrs) 
    throws InvalidRequestException
  {
    HeartbeatData dataManager = new DataFragment(HEARTBEAT_SERVER, 
            heartbeatGroupElement);

    dataManager.setHostData(attrs.getValue("host"));
    dataManager.saveData();
    heartbeatGroupElement.appendChild(dataManager.getFragment());
  }

  /**
   * 
   * @param namespaceURI
   * @param localName
   * @param rawName
   * @param attrs
   */
  public void startElement(String namespaceURI, String localName, 
          String rawName, Attributes attrs)
  {
    System.out.println("Starting element " + rawName);

    if (rawName.equals("deployedServers"))
    {
      return;
    }

    try
    {
      if (rawName.equals("global"))
      {
        handleGlobal(attrs);

        return;
      }
      else if (rawName.equals("featureServer"))
      {
        handleFeatureServer(attrs);

        return;
      }
      else if (rawName.equals("marshalServer"))
      {
        handleMarshalServer(attrs);

        return;
      }
      else if (rawName.equals("cdrServer"))
      {
        handleCdrServer(attrs);

        return;
      }
      else if (rawName.equals("redirectServer"))
      {
        handleRedirectServer(attrs);

        return;
      }
      else if (rawName.equals("heartbeatServer"))
      {
        handleHeartbeatServer(attrs);

        return;
      }
      else if (rawName.equals("policyServer"))
      {
        handlePdpServer(attrs);

        return;
      }
      else
      {
        System.out.println("Unrecognized server type: " + rawName);
      }
    }
    catch (InvalidRequestException e)
    {
      e.printStackTrace();
    }
  }

  /**
   */
  public void endDocument()
  {
    DOMToFile.writeAllFilesInTree(baseDocument);
  }

  /**
   * 
   * @param args
   */
  public static void main(String args[])
  {
    System.out.println("Starting DeployServers");

    if (args.length < 2)
    {
      System.out.println("Usage: DeployServers <host> <port>");

      return;
    }

    String host = args[0];
    int port = 0;

    try
    {
      port = Integer.parseInt(args[1]);
    }
    catch (NumberFormatException e)
    {
      System.out.println("Error: " + args[1] + " must be numeric");

      return;
    }

    try
    {
      connection = new VPPTransactionWrapper(host, port);
    }
    catch (VPPException e)
    {
      VPPTransactionWrapper.showVPPException(e, "Deployment failed");
    }

    DeployServers deployServers = new DeployServers();

    deployServers.processDeployFile();
  }


}




/*
Copyright (c) 2012 André Gröschel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TUIO;
namespace UniducialLibrary
{
    public class TuioManager : TuioListener
    {

        // Singleton Instance
        private static TuioManager m_Instance;
        private TuioClient m_Client;
        private List<TuioObject> m_TUIOObjects;

        public static TuioManager Instance
        {
            get
            {
                if (m_Instance == null)
                {
                    new TuioManager();
                }

                return m_Instance;
            }
        }




        public TuioManager()
        {
            if (m_Instance != null)
            {
                Debug.LogError("Trying to create two instances of singleton.");
                return;
            }

            m_Instance = this;

            m_Client = new TuioClient();
            m_Client.addTuioListener(this);

            //init members
            this.m_TUIOObjects = new List<TuioObject>();


        }

        ~TuioManager()
        {
            Disconnect();
        } 

        #region TUIOListener methods

        void TuioListener.addTuioObject(TuioObject in_TUIOObject)
        {
            this.m_TUIOObjects.Add(in_TUIOObject);
        }

        void TuioListener.updateTuioObject(TuioObject tobj)
        {

        }

        void TuioListener.removeTuioObject(TuioObject in_TUIOObject)
        {
            this.m_TUIOObjects.Remove(in_TUIOObject);
        }

        void TuioListener.addTuioCursor(TuioCursor tcur)
        {
            // throw new System.NotImplementedException();
        }

        void TuioListener.updateTuioCursor(TuioCursor tcur)
        {
            //throw new System.NotImplementedException();
        }

        void TuioListener.removeTuioCursor(TuioCursor tcur)
        {
            //throw new System.NotImplementedException();
        }

        void TuioListener.refresh(TuioTime ftime)
        {
            //throw new System.NotImplementedException();
        }
        #endregion

        public void Connect()
        {
            //setup TUIO client connection
            m_Client.connect();

            if (this.m_Client.isConnected())
            {

                Debug.Log("Listening to TUIO port " + m_Client.getPort() + ".");
            }
            else
            {
                Debug.LogError("Failed to connect to TUIO port " + m_Client.getPort() + ".");
            }
        }

        public bool IsMarkerAlive(int in_MarkerID)
        {

            foreach (TuioObject tuioObject in this.m_TUIOObjects)
            {
                if (tuioObject.getSymbolID() == in_MarkerID)
                {
                    return true;
                }
            }

            return false;
        }


        public TuioObject GetMarker(int in_MarkerID)
        {
            foreach (TuioObject tuioObject in this.m_TUIOObjects)
            {
                if (tuioObject.getSymbolID() == in_MarkerID)
                {
                    return tuioObject;
                }

            }

            return null;
        }

        public int GetObjectCount()
        {
            return this.m_TUIOObjects.Count;
        }

        public void Disconnect()
        {
            if (this.m_Client.isConnected())
            {
                int port = m_Client.getPort();
                m_Client.removeTuioListener(this);
                m_Client.disconnect();
                Debug.Log("Stopped listening to TUIO port " + port + ".");
            }
        }

        public bool IsConnected
        {
            get { return this.m_Client.isConnected(); }
        }

        public int TuioPort
        {
            get { return m_Client.getPort(); }
            set { m_Client.setPort(value); }
        }
    }
}
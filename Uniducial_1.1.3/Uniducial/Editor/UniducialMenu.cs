/*
Copyright (c) 2010 André Gröschel

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

using UnityEngine;
using System.Collections;
using UnityEditor;
using UniducialLibrary;

public class UniducialMenu : UnityEditor.Editor
{

    private const string FICUCIAL_COMPONENT_NAME = "FiducialController";

    [MenuItem("GameObject/Create Other/Fiducial Object")]
    //creates a new GameObject with a Fiducial Input component
    public static void CreateFiducialObject()
    {
        GameObject fiducialObject = new GameObject("FiducialObject");
        fiducialObject.AddComponent(typeof(Transform));
        fiducialObject.AddComponent(FICUCIAL_COMPONENT_NAME);
    }

    [MenuItem("Component/Input/Fiducial Controller")]

    //attaches a fiducial input component to all selected Game Objects
    public static void AttachFiducialController()
    {

        foreach (GameObject gameObject in Selection.gameObjects)
        {
            //make sure a GameObject can only be controlled by one marker
            if (gameObject.GetComponent(FICUCIAL_COMPONENT_NAME) != null)
            {
                Debug.LogWarning("Game Object " + gameObject.name + " already has a fiducial controller attached");
            }
            else
            {
                gameObject.AddComponent(FICUCIAL_COMPONENT_NAME);
            }
        }
    }

    private static bool sceneContaintsTUIO()
    {
        GameObject tuioGO = GameObject.Find("TUIO");

        if (tuioGO != null)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

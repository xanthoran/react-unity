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

using UnityEngine;
using System.Collections;
using UnityEditor;
using UniducialLibrary;

[CustomEditor(typeof(FiducialController))]
public class UniducialInspector : UnityEditor.Editor
{
    private FiducialController controller;

    public UniducialInspector()
    {




    }

    private void onEnable()
    {
        
    }

    public override void OnInspectorGUI()
    {
        controller = base.target as FiducialController;
        Camera mainCamera = GameObject.FindGameObjectWithTag("MainCamera").camera;

        EditorGUILayout.BeginHorizontal();
        controller.MarkerID = EditorGUILayout.IntField("Marker ID", controller.MarkerID);
        EditorGUILayout.EndHorizontal();
        EditorGUILayout.BeginHorizontal();
        controller.AutoHideGO = EditorGUILayout.Toggle("Auto-hide GameObject", controller.AutoHideGO);
        EditorGUILayout.EndHorizontal();

        EditorGUILayout.Separator();

        EditorGUILayout.BeginHorizontal();
        controller.IsPositionMapped
            = EditorGUILayout.Toggle("Control Position", controller.IsPositionMapped);
        EditorGUILayout.EndHorizontal();

        if (controller.IsPositionMapped)
        {

            EditorGUILayout.BeginHorizontal();
            controller.InvertX = EditorGUILayout.Toggle("Invert X-Axis", controller.InvertX);
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            controller.InvertY = EditorGUILayout.Toggle("Invert Y-Axis", controller.InvertY);
            EditorGUILayout.EndHorizontal();

            if (!mainCamera.isOrthoGraphic && !controller.isAttachedToGUIComponent())
            {
                EditorGUILayout.BeginHorizontal();
                controller.CameraOffset = EditorGUILayout.Slider("Camera offset", controller.CameraOffset, mainCamera.nearClipPlane, mainCamera.farClipPlane);
                EditorGUILayout.EndHorizontal();
            }
        }


        EditorGUILayout.Separator();

        if (!controller.isAttachedToGUIComponent())
        {
            EditorGUILayout.BeginHorizontal();
            controller.IsRotationMapped = EditorGUILayout.Toggle("Control Rotation", controller.IsRotationMapped);
            EditorGUILayout.EndHorizontal();

            if (controller.IsRotationMapped)
            {
                EditorGUILayout.BeginHorizontal();
                controller.RotateAround = (FiducialController.RotationAxis)EditorGUILayout.EnumPopup("Rotation Axis", controller.RotateAround);
                EditorGUILayout.EndHorizontal();

                EditorGUILayout.BeginHorizontal();
                controller.RotationMultiplier = EditorGUILayout.Slider("Rotation Factor", controller.RotationMultiplier, 0.01f, 5f);
                EditorGUILayout.EndHorizontal();
            }
        }


        if (GUI.changed)
            EditorUtility.SetDirty(controller);
    }
}

FacialExpression
======================
>**Designer:** Junbo Zhao, [homepage](https://zhaojunbo.github.io)

Introduction
-----
  This project focuses mainly on facial expression recognition, which has been widely used in varies of fields. Take ITS(Intelligent Transportation System) as an example, a good tool of facial analysis is able to judge the physical and pshychological condition of the driver and further reduce the possibility of hazard.     

Framework
-----
  My code is mainly comprised of three parts:      
    1. Find the positions of facial landmarks, including eyes, nose and mouth.    
    2. Extract LBP features over landmarks in a multi-scale and multi-cell manner. We normally ignore information from other parts of faces except the landmarks and their surroundings, since other parts would not bring us a discriminative classifier.      
    3. Calculate the difference between the face of a special expression and the neutral courterpart. Consequently the difference, namely a long vector, is that expression's feature.    
    4. Prepare lots of features following the steps above and Train a SVM.     
    5. Use the trained model to test a new face of an unknown expression.

Implementation details
-----
  To be updated!


Platform
------
  Ubuntu 12.04 LTS, g++ 4.6.3, OpenCV 2.4.7



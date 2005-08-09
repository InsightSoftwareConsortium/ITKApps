/*
 * ZIPFilter.java
 * 
 * Id
 * 
 * Version: $Revision$
 * 
 * Date: $Date$
 * 
 * Copyright (c) 2002, Hewlett-Packard Company and Massachusetts Institute of
 * Technology. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *  - Neither the name of the Hewlett-Packard Company nor the name of the
 * Massachusetts Institute of Technology nor the names of their contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
package org.dspace.app.mediafilter;

import java.awt.Graphics2D;
import javax.swing.*;
import java.awt.*;
import javax.imageio.*;
import java.util.*;
import java.io.*;
import java.util.zip.*;
import javax.activation.*;

import org.dspace.core.ConfigurationManager;

public class ZIPFilter extends MediaFilter 
{
     /**
     * @param filename
     *            string filename
     * 
     * @return string filtered filename
     */
     public String getFilteredName(String oldFilename)
     {
        return oldFilename + ".jpg";
     }
     /**
     * @return String bundle name
     *  
     */
     public String getBundleName()  
     {
        return "THUMBNAIL";
     }
     /**
     * @return String bitstreamformat
     */
    public String getFormatString()
    {
        return "JPEG";
    }
    /**
     * @return String description
     */
    public String getDescription()
    {
        return "Generated Thumbnail";
    }
    /**
     * @param source
     *            source input stream
     * 
     * @return InputStream the resulting input stream
     */
    public InputStream getDestinationStream(InputStream source)
            throws Exception
    {
     
       ZipInputStream zipp = new ZipInputStream( source );   
  
       if (zipp == null) 
	   {
       if (MediaFilterManager.isVerbose)
           {
             System.out.println (" Zip file 'zipp' is null ");
            }
       };
  
       // get the first entry from the archive
       ZipEntry entry = zipp.getNextEntry();
       String entryName = entry.getName();
       if (MediaFilterManager.isVerbose)
       {
            System.out.println (" The File Selected for Generating Thumbnail is " );
            System.out.println (" " + entryName);
            System.out.println ("  ");
       }

       // create a "temp" folder and give its path
       String filename = "/usr/local/dhaval/dicom/";
       OutputStream out = new  FileOutputStream ( filename + entryName); 
 
       // create a buffer
     
       byte[]  buffer = new byte[1024];  

       //  Transfer Bytes from input stream to Output DicomFile
       int len;
       while ((len = zipp.read(buffer)) >0 )
	   {
         out.write (buffer,0,len);  
       }
     
       out.close();
  
        // system call to the DicomReadWrite executable
       String[] cmd = 
	   {
          "/usr/local/dhaval/dicom/DicomGenerateThumbnail",
          filename + entryName,
         "/usr/local/dhaval/dicom/dhavaloutput.jpg"
       };
                                                                                                                             
       Process p =  Runtime.getRuntime().exec(cmd);
                                                                                                                             
       // put a BufferedReader on the output
                                                                                                                             
       InputStream inputstream = p.getInputStream();
       InputStreamReader inputstreamreader = new InputStreamReader(inputstream);
       BufferedReader bufferedreader = new BufferedReader(inputstreamreader);
                                                                                                                             
       // read the output
    
      File f = new File("/usr/local/dhaval/dicom/dicomheader.txt");
      BufferedWriter writer = new BufferedWriter(new FileWriter(f));
      
      writer.write(" The Header Contents of the File "+ entryName + " Selected for Generating Thumbnail is ");
      writer.newLine();         
                                                                                                         
      String line;
      while ((line = bufferedreader.readLine()) != null) 
      {
           if (MediaFilterManager.isVerbose)
           {
               System.out.println(line);
            }
           writer.write(line);
           writer.newLine(); //write system dependent end of line
      }
                              
      writer.close(); //close to unlock and flush to disk
                                                                                                                             
      // check for DicomReadWrite failure
                                                                                                                             
      try 
	  {
                if (p.waitFor() != 0) 
				{
                   if (MediaFilterManager.isVerbose)
                   System.err.println("exit value = " + p.exitValue());
                }
      }
      catch (InterruptedException e) 
	  {
              if (MediaFilterManager.isVerbose)
                System.err.println(e);
      }

      File fo = new File("/usr/local/dhaval/dicom/dhavaloutput.jpg");
      BufferedImage buf = ImageIO.read(fo);
      buf.flush();
        
      // get config params
      float xmax = (float) ConfigurationManager.getIntProperty("thumbnail.maxwidth");
      float ymax = (float) ConfigurationManager.getIntProperty("thumbnail.maxheight");

      Image thumb = null;

      // now get the image dimensions
      float xsize = (float) buf.getWidth(null);
      float ysize = (float) buf.getHeight(null);

      // if verbose flag is set, print out dimensions
      // to STDOUT
      if (MediaFilterManager.isVerbose)
        {
            System.out.println("original size: " + xsize + "," + ysize);
        }

      // scale by x first if needed
      if (xsize > xmax)
      {
            // calculate scaling factor so that xsize * scale = new size (max)
            float scale_factor = xmax / xsize;

            // if verbose flag is set, print out extracted text
            // to STDOUT
            if (MediaFilterManager.isVerbose)
            {
                System.out.println("x scale factor: " + scale_factor);
            }

            // now reduce x size
            // and y size
            xsize = xsize * scale_factor;
            ysize = ysize * scale_factor;

            // if verbose flag is set, print out extracted text
            // to STDOUT
            if (MediaFilterManager.isVerbose)
           {
                System.out.println("new size: " + xsize + "," + ysize);
           }
      }

      // scale by y if needed
      if (ysize > ymax)
      {
            float scale_factor = ymax / ysize;

            // now reduce x size
            // and y size
            xsize = xsize * scale_factor;
            ysize = ysize * scale_factor;
      }

      // if verbose flag is set, print details to STDOUT
      if (MediaFilterManager.isVerbose)
      {
            System.out.println("thumbnail size: " + xsize + ", " + ysize);
      }
     
      thumb = buf.getScaledInstance((int) xsize, (int) ysize,Image.SCALE_SMOOTH);

      // create a BufferedImage to draw this image into...
      BufferedImage thumbnail = new BufferedImage(thumb.getWidth(null), thumb.getHeight(null), BufferedImage.TYPE_BYTE_GRAY);

      Graphics2D g2d = thumbnail.createGraphics();

      // draw thumb into thumbnail
      g2d.drawImage(thumb, null, null);  
                     	  
      // now create an input stream and return it       
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
                                                                                                                               
      ImageIO.write(buf,"jpeg",baos); 

      // now get the array
      ByteArrayInputStream bais = new ByteArrayInputStream ( baos.toByteArray());
                                                                                    
      return bais;
   
   }
}


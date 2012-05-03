// micropolisdisasterspanel.cs
//
// Micropolis, Unix Version.  This game was released for the Unix platform
// in or about 1990 and has been modified for inclusion in the One Laptop
// Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
// you need assistance with this program, you may contact:
//   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.  You should have received a
// copy of the GNU General Public License along with this program.  If
// not, see <http://www.gnu.org/licenses/>.
//
//             ADDITIONAL TERMS per GNU GPL Section 7
//
// No trademark or publicity rights are granted.  This license does NOT
// give you any right, title or interest in the trademark SimCity or any
// other Electronic Arts trademark.  You may not distribute any
// modification of this program using the trademark SimCity or claim any
// affliation or association with Electronic Arts Inc. or its employees.
//
// Any propagation or conveyance of this program must include this
// copyright notice and these terms.
//
// If you convey this program (or any modifications of it) and assume
// contractual liability for the program to recipients of it, you agree
// to indemnify Electronic Arts for any liability that those contractual
// assumptions impose on Electronic Arts.
//
// You may not misrepresent the origins of this program; modified
// versions of the program must be marked as such and not identified as
// the original program.
//
// This disclaimer supplements the one included in the General Public
// License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
// PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
// OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
// SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
// DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
// INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
// FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
// RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
// USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
// INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
// MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
// WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
// CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
// ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
// JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
// WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
// CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
// NOT APPLY TO YOU.


////////////////////////////////////////////////////////////////////////
// Micropolis Disasters Panel
// Bil Simser

using System.Windows.Forms;

namespace MicropolisClient
{
    internal class MicropolisDisastersPanel : UserControl
    {
        private readonly MicropolisWinFormsEngine _engine;
        private Button buttonMonster;
        private Button buttonFire;
        private Button buttonFlood;
        private Button buttonTornado;
        private Button buttonMeltdown;
        private Button buttonEarthquake;
        private FlowLayoutPanel flowLayoutPanel1;

        public MicropolisDisastersPanel(MicropolisWinFormsEngine engine)
        {
            _engine = engine;
        }

        private void InitializeComponent()
        {
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.buttonMonster = new System.Windows.Forms.Button();
            this.buttonFire = new System.Windows.Forms.Button();
            this.buttonFlood = new System.Windows.Forms.Button();
            this.buttonTornado = new System.Windows.Forms.Button();
            this.buttonMeltdown = new System.Windows.Forms.Button();
            this.buttonEarthquake = new System.Windows.Forms.Button();
            this.flowLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.buttonMonster);
            this.flowLayoutPanel1.Controls.Add(this.buttonFire);
            this.flowLayoutPanel1.Controls.Add(this.buttonFlood);
            this.flowLayoutPanel1.Controls.Add(this.buttonTornado);
            this.flowLayoutPanel1.Controls.Add(this.buttonMeltdown);
            this.flowLayoutPanel1.Controls.Add(this.buttonEarthquake);
            this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(165, 89);
            this.flowLayoutPanel1.TabIndex = 0;
            // 
            // buttonMonster
            // 
            this.buttonMonster.Location = new System.Drawing.Point(3, 3);
            this.buttonMonster.Name = "buttonMonster";
            this.buttonMonster.Size = new System.Drawing.Size(75, 23);
            this.buttonMonster.TabIndex = 0;
            this.buttonMonster.Text = "Monster";
            this.buttonMonster.UseVisualStyleBackColor = true;
            this.buttonMonster.Click += new System.EventHandler(this.buttonMonster_Click);
            // 
            // buttonFire
            // 
            this.buttonFire.Location = new System.Drawing.Point(84, 3);
            this.buttonFire.Name = "buttonFire";
            this.buttonFire.Size = new System.Drawing.Size(75, 23);
            this.buttonFire.TabIndex = 1;
            this.buttonFire.Text = "Fire";
            this.buttonFire.UseVisualStyleBackColor = true;
            this.buttonFire.Click += new System.EventHandler(this.buttonFire_Click);
            // 
            // buttonFlood
            // 
            this.buttonFlood.Location = new System.Drawing.Point(3, 32);
            this.buttonFlood.Name = "buttonFlood";
            this.buttonFlood.Size = new System.Drawing.Size(75, 23);
            this.buttonFlood.TabIndex = 2;
            this.buttonFlood.Text = "Flood";
            this.buttonFlood.UseVisualStyleBackColor = true;
            this.buttonFlood.Click += new System.EventHandler(this.buttonFlood_Click);
            // 
            // buttonTornado
            // 
            this.buttonTornado.Location = new System.Drawing.Point(84, 32);
            this.buttonTornado.Name = "buttonTornado";
            this.buttonTornado.Size = new System.Drawing.Size(75, 23);
            this.buttonTornado.TabIndex = 3;
            this.buttonTornado.Text = "Tornado";
            this.buttonTornado.UseVisualStyleBackColor = true;
            this.buttonTornado.Click += new System.EventHandler(this.buttonTornado_Click);
            // 
            // buttonMeltdown
            // 
            this.buttonMeltdown.Location = new System.Drawing.Point(3, 61);
            this.buttonMeltdown.Name = "buttonMeltdown";
            this.buttonMeltdown.Size = new System.Drawing.Size(75, 23);
            this.buttonMeltdown.TabIndex = 4;
            this.buttonMeltdown.Text = "Meltdown";
            this.buttonMeltdown.UseVisualStyleBackColor = true;
            this.buttonMeltdown.Click += new System.EventHandler(this.buttonMeltdown_Click);
            // 
            // buttonEarthquake
            // 
            this.buttonEarthquake.Location = new System.Drawing.Point(84, 61);
            this.buttonEarthquake.Name = "buttonEarthquake";
            this.buttonEarthquake.Size = new System.Drawing.Size(75, 23);
            this.buttonEarthquake.TabIndex = 5;
            this.buttonEarthquake.Text = "Earthquake";
            this.buttonEarthquake.UseVisualStyleBackColor = true;
            this.buttonEarthquake.Click += new System.EventHandler(this.buttonEarthquake_Click);
            // 
            // MicropolisDisastersPanel
            // 
            this.Controls.Add(this.flowLayoutPanel1);
            this.Name = "MicropolisDisastersPanel";
            this.Size = new System.Drawing.Size(165, 89);
            this.flowLayoutPanel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        private void buttonMonster_Click(object sender, System.EventArgs e)
        {
            _engine.makeMonster();
        }

        private void buttonFire_Click(object sender, System.EventArgs e)
        {
            _engine.makeFire();
        }

        private void buttonFlood_Click(object sender, System.EventArgs e)
        {
            _engine.makeFlood();
        }

        private void buttonTornado_Click(object sender, System.EventArgs e)
        {
            _engine.makeTornado();
        }

        private void buttonMeltdown_Click(object sender, System.EventArgs e)
        {
            _engine.makeMeltdown();
        }

        private void buttonEarthquake_Click(object sender, System.EventArgs e)
        {
            _engine.makeEarthquake();
        }
    }
}